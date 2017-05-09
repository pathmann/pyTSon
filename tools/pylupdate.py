#!/usr/bin/env python3
"""
pylupdate parses python files to search for calls to pytson.tr resp
pytson.Translatable._tr to generate a Qt linguist translation file

These are the two valid usecases from module pytson:

class Translatable(object):
    @classmethod
    def _tr(cls, sourcetext, *, disambiguation="", n=-1, context=None):
        pass

def tr(context, sourcetext, *, disambiguation="", n=-1):
        pass
"""
import sys
import os

import ast

from argparse import ArgumentParser

import logging

from lxml import etree

from io import StringIO


"""
there is no valid dtd for Qt's ts files (because of the "wildcard" elemenents
extra-*), so drop that feature for validation
"""

DTD = r"""<!--
 !
 ! Some notes to the DTD:
 !
 ! The location element is set as optional since it was introduced first in
 ! Qt 4.2.
 ! The userdata element is set as optional since it was introduced first in
 ! Qt 4.4.
 ! The vanished message type was introduced first in Qt 5.2.
 !
  -->
<!--
 ! Macro used in order to escape byte entities not allowed in an xml document
 ! for instance, only #x9, #xA and #xD are allowed characters below #x20.
 -->
<!ENTITY % evilstring '(#PCDATA | byte)*' >
<!ELEMENT byte EMPTY>
<!-- value contains decimal (e.g. 1000) or hex (e.g. x3e8) unicode encoding of
 ! one char
  -->
<!ATTLIST byte
          value CDATA #REQUIRED>
<!--
 ! This element wildcard is no valid DTD. No better solution available.
 ! extra elements may appear in TS and message elements. Each element may
 ! appear
 ! only once within each scope. The contents are preserved verbatim; any
 ! attributes are dropped. Currently recognized extra tags include:
 !   extra-po-msgid_plural, extra-po-old_msgid_plural
 !   extra-po-flags (comma-space separated list)
 !   extra-loc-layout_id
 !   extra-loc-feature
 !   extra-loc-blank
 <!ELEMENT extra-* %evilstring; >
 <!ELEMENT TS (defaultcodec?, extra-**, dependencies?, (context|message)+) >
  -->
<!ELEMENT TS (defaultcodec?, dependencies?, (context|message)+) >
<!ATTLIST TS
          version CDATA #IMPLIED
          sourcelanguage CDATA #IMPLIED
          language CDATA #IMPLIED>
<!-- The encoding to use in the QM file by default. Default is ISO-8859-1. -->
<!ELEMENT defaultcodec (#PCDATA) >
<!ELEMENT context (name, comment?, (context|message)+) >
<!ATTLIST context
          encoding CDATA #IMPLIED>
<!ELEMENT dependencies (dependency+) >
<!ATTLIST dependency
          catalog CDATA #IMPLIED>
<!ELEMENT name %evilstring; >
<!-- This is "disambiguation" in the (new) API, or "msgctxt" in gettext speak
  -->
<!ELEMENT comment %evilstring; >
<!-- Previous content of comment (result of merge) -->
<!ELEMENT oldcomment %evilstring; >
<!-- The real comment (added by developer/designer) -->
<!ELEMENT extracomment %evilstring; >
<!-- Comment added by translator -->
<!ELEMENT translatorcomment %evilstring; >
<!-- <!ELEMENT message (location*, source?, oldsource?, comment?, oldcomment?,
 ! extracomment?, translatorcomment?, translation?, userdata?, extra-**) >
  -->
<!ELEMENT message (location*, source?, oldsource?, comment?, oldcomment?,
extracomment?, translatorcomment?, translation?, userdata?) >
<!--
 ! If utf8 is "true", the defaultcodec is overridden and the message is encoded
 ! in UTF-8 in the QM file. If it is "both", both source encodings are stored
 ! in the QM file.
  -->
<!ATTLIST message
          id CDATA #IMPLIED
          utf8 (true|false|both) "false"
          numerus (yes|no) "no">
<!ELEMENT location EMPTY>
<!--
 ! If the line is omitted, the location specifies only a file.
 !
 ! location supports relative specifications as well. Line numbers are
 ! relative (explicitly positive or negative) to the last reference to a
 ! given filename; each file starts with current line 0. If the filename
 ! is omitted, the "current" one is used. For the 1st location in a message,
 ! "current" is the filename used for the 1st location of the previous message.
 ! For subsequent locations, it is the filename used for the previous location.
 ! A single TS file has either all absolute or all relative locations.
  -->
<!ATTLIST location
          filename CDATA #IMPLIED
          line CDATA #IMPLIED>
<!ELEMENT source %evilstring;>
<!-- Previous content of source (result of merge) -->
<!ELEMENT oldsource %evilstring;>
<!--
 ! The following should really say one evilstring macro or several
 ! numerusform or lengthvariant elements, but the DTD can't express this.
  -->
<!ELEMENT translation (#PCDATA|byte|numerusform|lengthvariant)* >
<!--
 ! If no type is set, the message is "finished".
 ! Length variants must be ordered by falling display length.
 ! variants may not be yes if the message has numerus yes.
  -->
<!ATTLIST translation
          type (unfinished|vanished|obsolete) #IMPLIED
          variants (yes|no) "no">
<!-- Deprecated. Use extra-* -->
<!ELEMENT userdata (#PCDATA)* >
<!--
 ! The following should really say one evilstring macro or several
 ! lengthvariant elements, but the DTD can't express this.
 ! Length variants must be ordered by falling display length.
  -->
<!ELEMENT numerusform (#PCDATA|byte|lengthvariant)* >
<!ATTLIST numerusform
          variants (yes|no) "no">
<!ELEMENT lengthvariant %evilstring; >
"""


class Message(object):
    """
    Wrapper for a translated message
    """

    def __init__(self, sourcetext, disambiguation, nused, finished=False):
        """
        Instantiates a new Message object
        @param sourcetext: sourcetext
        @type sourcetext: str
        @param disambiguation: string to distinguish between two equal
        sourcetexts
        @type disambiguation: str or None
        @param nused: if True, numerous translations will be used
        @type nused: bool
        @param finished: defines, whether the translation is finished.
        defaults to False
        @type finished: bool
        """
        self.sourcetext = sourcetext
        self.disambiguation = disambiguation
        self.isNumerous = nused
        self.isFinished = finished
        self.translations = [""]

    @property
    def sourcetext(self):
        return self._sourcetext

    @sourcetext.setter
    def sourcetext(self, val):
        self._sourcetext = val

    @property
    def disambiguation(self):
        return self._disambiguation

    @disambiguation.setter
    def disambiguation(self, val):
        self._disambiguation = val

    @property
    def isNumerous(self):
        return self._numerous

    @isNumerous.setter
    def isNumerous(self, val):
        self._numerous = val

    @property
    def isFinished(self):
        return self._finished

    @isFinished.setter
    def isFinished(self, val):
        self._finished = val

    def __iter__(self):
        """
        Yields the translations of the Message
        @return: the translated string(s)
        @rtype: str
        """
        for t in self.translations:
            yield t

    def __repr__(self):
        return "Message(%s, %s)" % (self._sourcetext, self._disambiguation)

    def setTranslation(self, trans, numeroustrans=None):
        """
        Sets the translation of the Message
        @param trans: the translation
        @type trans: str
        @param numeroustrans: if not None, the numerous translation
        @type numeroustrans: str or None
        """
        self.translations = [trans]
        if numeroustrans:
            self.translations.append(numeroustrans)
            self._numerous = True
        else:
            self._numerous = False

    def update(self, msg):
        """
        Updates the properties with the ones from another message
        @param msg: the other message
        @type msg: Message
        """
        self.translations = msg.translations

        if msg.isNumerous:
            self.isNumerous = True

        if msg.isFinished:
            self.isFinished = True

    @staticmethod
    def fromXml(elem):
        """
        Parses the xml element message to a Message object
        @param elem: the xml element
        @type elem: ElementTree.Element
        @return: a new created message object
        @rtype: Message
        """
        nused = "numerus" in elem.attrib and elem.attrib["numerus"] == "yes"
        sourcetext = elem.find("source").text
        disambiguation = None
        disel = elem.find("comment")
        if disel:
            disambiguation = disel.text

        transel = elem.find("translation")

        msg = Message(sourcetext, disambiguation, nused,
                      "type" not in transel.attrib)

        if not nused:
            msg.setTranslation(transel.text or "")
        else:
            msg.setTranslation(*[x.text or "" for x in
                                 transel.findall("numerusform")])

        return msg

    def toXml(self):
        """
        Creates the xml elements of the message
        @return: the xml element
        @rtype: ElementTree.Element
        """
        if self.isNumerous:
            elem = etree.Element("message", attrib={"numerus": "yes"})
        else:
            elem = etree.Element("message")

        etree.SubElement(elem, "source").text = self.sourcetext

        if self.disambiguation:
            etree.SubElement(elem, "comment").text = self.disambiguation

        if self.isFinished:
            trans = etree.SubElement(elem, "translation")
        else:
            trans = etree.SubElement(elem, "translation",
                                     attrib={"type": "unfinished"})

        if self.isNumerous:
            etree.SubElement(trans, "numerusform").text = self.translations[0]
            nfel = etree.SubElement(trans, "numerusform")
            if len(self.translations) > 1:
                nfel.text = self.translations[1]
        else:
            trans.text = self.translations[0]

        return elem


class Context(object):
    """
    Wrapper for a translation context
    """

    def __init__(self):
        """
        Instantiates a new Context object
        """
        self.msgs = {}

    def __iter__(self):
        """
        Yields each Message object in the context
        @return: the message objects
        @rtype: Message
        """
        for msg in self.msgs.values():
            yield msg

    def __repr__(self):
        return "Context(%s)" % ", ".join(str(x) for x in self.msgs.values())

    def addMessage(self, msg):
        """
        Adds a message to the context. If already exists, updates it.
        @param msg: the message
        @type msg: Message
        """
        if (msg.sourcetext, msg.disambiguation) in self.msgs:
            self.msgs[(msg.sourcetext, msg.disambiguation)].update(msg)
        else:
            self.msgs[(msg.sourcetext, msg.disambiguation)] = msg

    def update(self, ctx):
        """
        Updates all current messages with data from another
        context (if contained).
        @param ctx: the other context
        @type ctx: Context
        """
        for msg in ctx:
            if (msg.sourcetext, msg.disambiguation) in self.msgs:
                self.msgs[(msg.sourcetext, msg.disambiguation)].update(msg)

    @staticmethod
    def fromXml(elem):
        """
        Creates a context object from the xml element
        @param elem: the xml element
        @type elem: ElementTree.Element
        @return: a tuple containing the name and the new context object
        @rtype: tuple(str, Context)
        """
        name = elem.find("name").text

        ctx = Context()
        for subelem in elem.findall("message"):
            ctx.addMessage(Message.fromXml(subelem))

        return (name, ctx)

    def toXml(self, name):
        """
        Creates the xml elements of the context
        @param name: the name of the context
        @type name: str
        @return: the xml element
        @rtype: ElementTree.Element
        """
        elem = etree.Element("context")
        etree.SubElement(elem, "name").text = name

        for msg in sorted(self.msgs.values(), key=lambda x: x.sourcetext):
            elem.append(msg.toXml())

        return elem


class Translation(object):
    """
    Wrapper for a Qt linguist translation file
    """

    def __init__(self, filename, language):
        """
        Instantiates a new Translation object
        @param filename: path to read from resp. write to
        @type filename: str
        @param language: the target language code
        @type language: str
        """
        self.contexts = {}

        self.filename = filename
        self.language = language

    def _loadXml(self):
        """
        Parses the xml document from self.filename and creates corresponding
        context objects
        """
        tree = etree.ElementTree(file=self.filename)
        if tree.getroot().tag != "TS":
            raise Exception("No valid translation file (roottag)")

        for ele in tree.getroot().findall("context"):
            (name, ctx) = Context.fromXml(ele)
            self.contexts[name] = ctx

    def read(self, filename=None):
        """
        Read the translation file given by the filename
        @param filename: if given, the path to the file to read.
        Defaults to None
        @type filename: str
        """
        if filename:
            self.filename = filename

        self._loadXml()

    def write(self, language, dtd=None, filename=None):
        """
        Writes the data to a file.
        @param language: the target language code
        @type language: str
        @param dtd: if set, the xml is validated before written, defaults
        to None, throws Exception if validation failed
        @type dtd: str
        @param filename: if given, the path to write to, defaults to None
        @type filename: str
        """
        if filename:
            self.filename = filename

        root = etree.Element("TS", attrib={"version": "2.1",
                                           "language": language})

        for ctxname in sorted(self.contexts.keys()):
            root.append(self.contexts[ctxname].toXml(ctxname))

        tree = etree.ElementTree(root)

        if dtd:
            val = etree.DTD(StringIO(dtd))
            if not val.validate(tree):
                raise Exception("Error validating: %s" %
                                val.error_log.filter_from_errors()[0])

        tree.write(self.filename, encoding='utf-8', xml_declaration=True,
                   pretty_print=True)

    def __contains__(self, key):
        """
        Checks, if a context is contained
        @param key: the context name
        @type key: str
        @return: returns True, if a context is contained by key's name
        @rtype: bool
        """
        return key in self.contexts

    def __getitem__(self, key):
        """
        Returns the context object references by its name
        @param key: the name of the context
        @type key: str
        @return: the context
        @rtype: Context
        """
        return self.contexts[key]

    def __iter__(self):
        """
        Yields each context name contained in the translation
        @return: the context names
        @rtype: str
        """
        for ctx in self.contexts:
            yield ctx

    def addContext(self, key):
        """
        Creates a new context. A previous context with that name will
        be overwritten
        @param key: the name of the context
        @type key: str
        """
        self.contexts[key] = Context()

    def removeContext(self, key):
        """
        Remove a context.
        @param key: the name of the context
        @type key: str
        """
        del self.contexts[key]


class ParentVisitor(ast.NodeVisitor):
    """
    ast Visitor which sets links to the parent node
    """

    def generic_visit(self, node):
        if not hasattr(node, "parent"):
            node.parent = None

        for _, child in ast.iter_fields(node):
            if isinstance(child, ast.AST):
                child.parent = node
            elif isinstance(child, list):
                for item in child:
                    item.parent = node

        super().generic_visit(node)


class FunctionValidator(ParentVisitor):
    """
    ast Visitor to find calls to pyTSon's translation functions
    """

    def __init__(self):
        super().__init__()

        self.modname = None
        self.classname = None
        self.funcname = None

        self.calls = []

        self.log = logging.getLogger("pylupdate")

    def _hasImport(self):
        """
        Checks, if the translation functions were already imported
        @return: Returns True, if any function is available
        @rtype: bool
        """
        return any([self.modname, self.classname, self.funcname])

    def _getClass(self, node):
        """
        Returns the parent class of some ast node
        @param node: the node to get the parent of (must be previously visited
        with ParentVisitor)
        @type node: ast.AST
        @return: the node of the parent class
        @rtype: ast.ClassDef
        """
        parent = node.parent
        while parent:
            if isinstance(parent, ast.ClassDef):
                return parent
            parent = parent.parent

        return None

    def _classIsTranslatable(self, node):
        """
        Checks, if a class is a subclass of pytson.Translatable
        @param node: the node of the classdef
        @type node: ast.ClassDef
        @return: True or False
        @rtype: bool
        """
        for base in node.bases:
            if isinstance(base, ast.Attribute):
                if "%s.%s" % (base.value.id, base.attr) == self.classname:
                    return True
            elif isinstance(base, ast.Name):
                if base.id == self.classname:
                    return True
        return False

    def _getKeywords(self, node):
        """
        Gets the keyword parameters of a translate function call
        @param node:
        @type node: ast.AST
        @return: a tuple containing the contextname, the source string,
        the disambiguation string and if the numerous form is requested
        @rtype: tuple(str, str or None, bool)
        """
        ctx = None
        disambiguation = None
        nused = False
        sourcetext = None

        for kw in node.keywords:
            if kw.arg == "context":
                if not isinstance(kw.value, ast.Str):
                    self.log.debug("Type of argument context needs to be raw "
                                   "string (line %s)" % node.lineno)
                    return None
                else:
                    ctx = kw.value.s
            elif kw.arg == "disambiguation":
                if not isinstance(kw.value, ast.Str):
                    self.log.debug("Type of argument disambiguation needs to "
                                   "be raw string (line %s)" % node.lineno)
                    return None
                else:
                    disambiguation = kw.value.s
            elif kw.arg == "n":
                nused = True
            elif kw.arg == "sourcetext":
                if not isinstance(kw.value, ast.Str):
                    self.log.debug("Type of argument sourcetext needs to "
                                   "be raw string (line %s)" % node.lineno)
                    return None
                else:
                    sourcetext = kw.value.s
            else:
                self.log.debug("Unknown keyword %s (line %s)" %
                               (kw.arg, node.lineno))
                return None

        return (ctx, sourcetext, disambiguation, nused)

    @staticmethod
    def _checkArguments(node, count):
        """
        Checks, if the argument list of a translation function is valid
        @param node: the node of the call
        @type node: ast.AST
        @param count: number of arguments to check for
        @type count: int
        @return: True or False
        @rtype: bool
        """
        if len(node.args) != count:
            return False

        for i in range(count):
            if not isinstance(node.args[i], ast.Str):
                return False

        return True

    def _extractMethodCall(self, node):
        """
        Extracts all info from a Translatable._tr call
        @param node: the method call
        @type node: ast.Attribute
        @return: a tuple containing the contextname and the message
        or None, if the call is not valid
        @rtype: tuple(str, str, str or None, bool) or None
        """
        call = "%s.%s" % (node.func.value.id, node.func.attr)
        if call in ["self._tr", "cls._tr"]:
            (ctx, source, disambiguation, nused) = self._getKeywords(node)
            if not ctx:
                cl = self._getClass(node)
                if not cl:
                    self.log.debug("No context found (line %s)" % node.lineno)
                    return None

                if self._classIsTranslatable(cl):
                    ctx = cl.name
                else:
                    self.log.debug("Class is not translatable (line %s)" %
                                   node.lineno)

            # if source was not given as keyword arg
            if not source:
                if not self._checkArguments(node, 1):
                    self.log.debug("Argument list not matched (line %s)" %
                                   node.lineno)
                    return None
                else:
                    source = node.args[0].s

            return (ctx, Message(source, disambiguation, nused))

        return None

    def _extractModuleCall(self, node):
        """
        Extracts all info from a pytson.tr call
        @param node: the function call
        @type node: ast.Attribute
        @return: a tuple containing the contextname and the message
        or None, if the call is not valid
        @rtype: tuple(str, Message) or None
        """
        if "%s.%s" % (node.func.value.id, node.func.attr) == self.funcname:
            (ctx, source, disambiguation, nused) = self._getKeywords(node)

            count = [ctx, source].count(None)
            if count > 0 and not self._checkArguments(node, count):
                self.log.debug("Argument list not matched (line %s)" %
                               node.lineno)
                return None

            # if source was no keyword, context can't be either
            if not source:
                if ctx:
                    self.log.debug("Argument list not matched, wrong order "
                                   "(line %s)" % node.lineno)
                    return None

                ctx = node.args[0].s
                source = node.args[1].s
            elif not ctx:
                ctx = node.args[0].s

            return (ctx, Message(source, disambiguation, nused))

        return None

    def _extractFunctionCall(self, node):
        """
        Extracts all info from a tr call (imported with alias)
        @param node: the function call
        @type node: ast.Attribute
        @return: a tuple containing the contextname and the message
        or None, if the call is not valid
        @rtype: tuple(str, Message) or None
        """
        if node.func.id == self.funcname:
            (ctx, source, disambiguation, nused) = self._getKeywords(node)

            count = [ctx, source].count(None)
            if count > 0 and not self._checkArguments(node, count):
                self.log.debug("Argument list not matched (line %s)" %
                               node.lineno)
                return None

            # if source was no keyword, context can't be either
            if not source:
                if ctx:
                    self.log.debug("Argument list not matched, wrong order "
                                   "(line %s)" % node.lineno)
                    return None

                ctx = node.args[0].s
                source = node.args[1].s
            elif not ctx:
                ctx = node.args[0].s

            return (ctx, Message(source, disambiguation, nused))

        return None

    def _validateExtract(self, node):
        """
        Extracts all info from a translation call
        @param node: the method call
        @type node: ast.Attribute
        @return: a tuple containing the contextname and the message
        or None, if the call is not valid
        @rtype: tuple(str, str, str or None, bool) or None
        """
        if not self._hasImport():
            return None

        if (isinstance(node.func, ast.Attribute) and
           isinstance(node.func.value, ast.Name)):
            args = self._extractMethodCall(node)
            if args:
                return args

            args = self._extractModuleCall(node)
            if args:
                return args
        elif isinstance(node.func, ast.Name):
            args = self._extractFunctionCall(node)
            if args:
                return args

        return None

    def visit_Import(self, node):
        """
        Visits each import of the ast
        @param node: the import node
        @type node: ast.Import
        """
        for imp in node.names:
            if imp.name == "pytson":
                if imp.asname:
                    self.modname = imp.asname
                    if not self.funcname:
                        self.funcname = "%s.tr" % imp.asname
                    if not self.classname:
                        self.classname = "%s.Translatable" % imp.asname
                else:
                    self.modname = "pytson"
                    if not self.funcname:
                        self.funcname = "pytson.tr"
                    if not self.classname:
                        self.classname = "pytson.Translatable"

    def visit_ImportFrom(self, node):
        """
        Visits each import (from) of the ast
        @param node: the import node
        @type node: ast.ImportFrom
        """
        if node.module == "pytson":
            for imp in node.names:
                if imp.name == "tr":
                    if imp.asname:
                        self.funcname = imp.asname
                    else:
                        self.funcname = "tr"
                elif imp.name == "Translatable":
                    if imp.asname:
                        self.classname = imp.asname
                    else:
                        self.classname = "Translatable"

    def visit_Call(self, node):
        """
        Visits each function call of the ast
        @param node: the call node
        @type node: ast.Call
        """
        args = self._validateExtract(node)

        if args:
            self.calls.append(args)
        else:
            self.generic_visit(node)


def _findallRecursive(node, path):
    """
    Finds all matching subelements, by tag name or path, searches recursively
    @param node: the xml element to search in
    @type node: ElementTree.Element
    @param path: what element to look for
    @type path: str
    @return: yields each matching element
    @rtype: ElementTree.Element
    """
    for ele in node.getchildren():
        if ele.tag == path:
            yield ele
        for sub in _findallRecursive(ele, path):
            yield sub


def _getUiTexts(inputfile):
    """
    Extracts all translatable string properties from a Qt ui file
    @param inputfile: path to the ui file
    @type inputfile: str
    @return: a list of tuples, containing the contextname and the message
    @rtype: list[tuple(str, Message)]
    """
    tree = etree.ElementTree(file=inputfile)
    if tree.getroot().tag != "ui":
        raise Exception("No valid uifile")

    ctx = tree.find("class").text
    ret = []

    for ele in _findallRecursive(tree.getroot(), "string"):
        if "notr" not in ele.attrib or ele.attrib["notr"] == "false":
            if "comment" in ele.attrib:
                msg = Message(ele.text or "", ele.attrib["comment"], False)
            else:
                msg = Message(ele.text or "", None, False)

            ret.append((ctx, msg))

    return ret


def getSourceTexts(inputfile):
    """
    Extracts all translate function info from a python source file or
    Qt ui file
    @param inputfile: path to the python source file/ui file
    @type inputfile: str
    @return: a list of tuples, containing the contextname and the message
    @rtype: list[tuple(str, Message)]
    """
    logging.getLogger("pylupdate").info("Parsing %s" % inputfile)

    if os.path.splitext(inputfile)[1] == ".py":
        tree = ast.parse(open(inputfile).read())

        val = FunctionValidator()
        val.visit(tree)

        ret = val.calls
    else:
        ret = _getUiTexts(inputfile)

    logging.getLogger("pylupdate").debug("Found sources: %s" % ret)
    return ret


def main(argv):
    """
    Main function of pylupdate
    @param argv: the arguments passed to the scripts
    @type argv: list[str]
    """
    parser = ArgumentParser(argv)
    parser.add_argument('input', help='Input file or directory')
    parser.add_argument('output', help='The file to write to')
    parser.add_argument('-l', '--language', dest='language', action='store',
                        default='de_DE', help='language code')
    parser.add_argument('-e', '--exclude', dest='excludes', action='store',
                        nargs='*', help='Ignore file(s)', default=[])
    parser.add_argument('-f', '--force', dest='force', action='store_true',
                        help='Force rewrite of outputfile if already exists')
    parser.add_argument('-n', '--no-obsolete', dest='clear',
                        action='store_true',
                        help='Delete sourcetexts not found in contexts')
    parser.add_argument('-c', '--check', dest='check', action='store_true',
                        help='validate output file against DTD')
    parser.add_argument('-v', '--verbose', dest='verbose', action='store_true',
                        help='Verbose output')

    args = parser.parse_args()

    log = logging.getLogger("pylupdate")
    ch = logging.StreamHandler()
    log.addHandler(ch)

    if args.verbose:
        log.setLevel(logging.DEBUG)
    else:
        log.setLevel(logging.INFO)

    if os.path.isfile(args.input):
        files = [args.input]
    elif os.path.isdir(args.input):
        files = []
        for root, dirnames, filenames in os.walk(args.input):
            for f in filenames:
                if os.path.splitext(f)[1] in [".py", ".ui"]:
                    files.append(os.path.join(root, f))
    else:
        raise Exception("input has to be a valid file- or directorypath")

    trans = Translation(args.output, args.language)
    if os.path.isfile(args.output) and not args.force:
        log.info("There might be information loss by reading a translation "
                 "file generated by other tools")
        trans.read()

    contexts = {}
    for f in filter(lambda x: x not in args.excludes, files):
        for (ctxname, msg) in getSourceTexts(f):
            if args.clear and ctxname not in contexts:
                if ctxname in trans:
                    contexts[ctxname] = trans[ctxname]
                    trans.addContext(ctxname)
                else:
                    contexts[ctxname] = None

            if ctxname not in trans:
                trans.addContext(ctxname)

            trans[ctxname].addMessage(msg)

    for name, ctx in contexts.items():
        if ctx:
            trans[name].update(ctx)

    if args.clear:
        for name in list(trans):
            if name not in contexts:
                trans.removeContext(name)

    trans.write(args.language, DTD if args.check else None)


if __name__ == "__main__":
    main(sys.argv)
