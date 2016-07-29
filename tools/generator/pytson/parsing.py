import os
import logging

from pycparser import parse_file, c_ast, c_generator
from .wrappers import FunctionWrapper, CallbackWrapper


def maximumPrefix(str1, str2):
    if len(str1) < len(str2):
        return maximumPrefix(str2, str1)

    for i, c in enumerate(str1):
        if c != str2[i]:
            return str1[:i]

    return str1


class ts3Visitor(c_ast.NodeVisitor):
    """

    """
    def __init__(self):
        self.enums = {}
        self.constants = {}
        self.callbacks = {}
        self.functions = {}
        self.curpar = None

    def findEnumValue(self, name):
        """

        :param name:
        :return:
        """
        for enumname in self.enums:
            for (ename, val) in self.enums[enumname]:
                if ename == name:
                    return val

        return None

    def getEnumValue(self, cvalue, siblings):
        """

        :param cvalue:
        :param siblings:
        :return:
        """
        if cvalue:
            if type(cvalue) is c_ast.Constant:
                return cvalue.value
            elif type(cvalue) is c_ast.ID:
                val = self.findEnumValue(cvalue.name)
                if val is not None:
                    return val
                else:
                    logging.warning("Could not evaluate ENUM %s" % cvalue.name)
                    return cvalue.name
            else:
                return c_generator.CGenerator().visit(cvalue)
        else:
            if len(siblings) == 0:
                return 0
            else:
                try:
                    return eval("%s +1" % siblings[-1][1])
                except NameError:
                    return "%s +1" % siblings[-1][1]

    def visit_Enum(self, node):
        """

        :param node:
        :return:
        """
        if type(self.curpar) is c_ast.Decl:
            name = node.name

            if name in self.enums:
                return

            if node.values:
                vals = []
                for i, enumerator in enumerate(node.values.enumerators):
                    vals.append((enumerator.name, self.getEnumValue(enumerator.value, vals)))

                if name is None:
                    if len(vals) >= 2:
                        name = maximumPrefix(vals[0][0], vals[1][0])

                    if name == '' or len(vals) == 1:
                        name = vals[0][0] + "_ENUM"
                    else:
                        if name.endswith("_"):
                            name = name[:-1]

                self.enums[name] = vals

    def visit_Constant(self, node):
        """

        :param node:
        :return:
        """
        if type(self.curpar) is c_ast.Decl:
            self.constants[self.curpar.name] = node.value

    def visit_FuncDecl(self, node):
        """

        :param node:
        :return:
        """
        t = node.type
        while (type(t) == c_ast.PtrDecl):
            t = t.type

        #we don't want the declarations of callbacks from plugin.h here
        if not t.declname.startswith('ts3plugin_'):
            self.functions[t.declname] = FunctionWrapper(ts3decl=node)

    def visit_FuncDef(self, node):
        name = node.decl.name
        if name.startswith('ts3plugin_'):
            self.callbacks[name[10:]] = CallbackWrapper(decl=node, original=True)

    def generic_visit(self, node):
        """

        :param node:
        :return:
        """
        oldpar = self.curpar
        self.curpar = node
        super(ts3Visitor, self).generic_visit(node)
        self.curpar = oldpar


class pyTSonVisitor(c_ast.NodeVisitor):
    """

    """
    def __init__(self, targets=[]):
        self.constants = {}
        self.enums = {}
        self.functions = {}
        self.callbacks = {}

    def visit_FuncDef(self, node):
        """

        :param node:
        :return:
        """
        name = node.decl.name
        if name.startswith('ts3plugin_'):
            #load callback
            self.callbacks[name[10:]] = CallbackWrapper(decl=node, original=False)
        else:
            #load function
            if name not in self.callbacks:
                self.functions[name] = FunctionWrapper(pytsondecl=node.decl)


def loadSymbols(filename, cppargs = []):
    """

    :param filename:
    :param cppargs:
    :return:
    """
    return parse_file(filename, use_cpp=True, cpp_args=cppargs)


def getSymbols(fpath, incldirs, original=True):
    """

    :param fpath:
    :param incldir:
    :param original
    :return:
    """
    visitor = ts3Visitor() if original else pyTSonVisitor()

    cppargs = [r'-DPYTSON_PARSER', r'-D__attribute__(a)=', r'-I%s' % os.path.join(os.path.abspath(os.path.dirname(__file__)), "fake_libc_include"), r'-I%s' % os.path.join(os.path.abspath(os.path.dirname(__file__)), "fake_pyTSon")] + [r'-I%s' % p for p in incldirs]
    symbs = loadSymbols(fpath, cppargs)

    visitor.visit(symbs)

    return visitor.constants, visitor.enums, visitor.callbacks, visitor.functions