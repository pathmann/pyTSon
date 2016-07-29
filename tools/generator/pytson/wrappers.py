from pycparser import c_generator, c_ast

cgen = c_generator.CGenerator()

FILESTORAGE = {}


def closeFiles():
    global FILESTORAGE
    FILESTORAGE = {}


class FunctionWrapper(object):
    """

    """
    def __init__(self, ts3decl=None, pytsondecl=None):
        """

        :param ts3decl:
        :param pytsondecl:
        """
        if not ts3decl and not pytsondecl:
            raise Exception("No declaration given")

        self.ts3decl = ts3decl
        self.pytsondecl = pytsondecl
        self.bodylines = []

        #if we generate from previous code, extract the body
        #to work, developers have to use correct indentation! (function-closing line must be "}" without any leading spaces)
        if self.pytsondecl:
            fname = self.pytsondecl.coord.file
            if not fname in FILESTORAGE:
                with open(fname, "r") as f:
                    FILESTORAGE[fname] = f.read().split('\n')

            cur = self.pytsondecl.coord.line
            abort = False
            while not abort:
                if FILESTORAGE[fname][cur].rstrip() == "}":
                    abort = True
                else:
                    self.bodylines.append(FILESTORAGE[fname][cur])
                    cur += 1

    @property
    def name(self):
        if self.ts3decl:
            return self.ts3decl.type.declname
        else:
            return self.pytsondecl.name

    @property
    def signature(self):
        if self.ts3decl:
            return cgen.visit(self.ts3decl)
        else:
            if len(self.bodylines) == 0:
                return ""

            sig = self.bodylines[0].strip()
            if not sig.startswith("//"):
                return ""
            else:
                return sig[2:]

    @property
    def parameterNames(self):
        if self.ts3decl:
            if not self.ts3decl.args:
                return []

            return [p.name for p in self.ts3decl.args.params]
        else:
            return [p.arg for p in self.pydecl.args.args]

    @property
    def body(self):
        if self.ts3decl:
            return ""
        else:
            return '\n'.join(self.bodylines)


class CallbackWrapper(object):
    """

    """
    def __init__(self, decl, original=True):
        """

        :param decl:
        :param original:
        """
        self.decl = decl
        self.original = original
        self.bodylines = []

        # if we generate from previous code, extract the body
        # to work, developers have to use correct indentation! (function-closing line must be "}" without any leading spaces)
        if not self.original:
            fname = self.decl.coord.file
            if not fname in FILESTORAGE:
                with open(fname, "r") as f:
                    FILESTORAGE[fname] = f.read().split('\n')

            cur = self.decl.coord.line
            abort = False
            while not abort:
                if FILESTORAGE[fname][cur].rstrip() == "}":
                    abort = True
                else:
                    self.bodylines.append(FILESTORAGE[fname][cur])
                    cur += 1

    @staticmethod
    def parameterType(param):
        if type(param.type) is c_ast.PtrDecl:
            return "*" + CallbackWrapper.parameterType(param.type)
        elif type(param.type.type) is c_ast.Enum:
            return "enum"
        else:
            return " ".join(param.type.type.names)

    @staticmethod
    def convertParameter(t):
        if t == "uint64":
            return "(unsigned long long)"
        elif t == "anyID":
            return "(unsigned int)"
        elif t == "enum":
            return "(unsigned int)"
        else:
            return ""

    @staticmethod
    def formatChar(param):
        t = CallbackWrapper.parameterType(param)

        if t == "uint64":
            return "K"
        elif t == "anyID" or t == "unsigned int":
            return "I"
        elif t == "*char":
            return "s"
        elif t == "int":
            return "i"
        elif t == "float":
            return "f"
        elif t == "char":
            return "c"
        elif t == "short":
            return "h"
        elif t == "enum":
            return "I"
        else:
            raise Exception("Unrecognized parametertype in formatChar: %s" % t)

        return ""

    @property
    def name(self):
        return self.decl.decl.name

    @property
    def signature(self):
        return cgen.visit(self.decl.decl)

    @property
    def convertedParameters(self):
        if not self.decl.decl.type.args:
            return []

        return ["%s %s" % (self.convertParameter(self.parameterType(p)), p.name) for p in self.decl.decl.type.args.params]


    @property
    def parameterNames(self):
        if not self.decl.decl.type.args:
            return []

        return [p.name if p.name != "type" else "atype" for p in self.decl.decl.type.args.params]

    @property
    def formatString(self):
        if not self.decl.decl.type.args:
            return ""

        ret = ""
        for p in self.decl.decl.type.args.params:
            ret += self.formatChar(p)

        return ret

    @property
    def body(self):
        if self.original:
            return ""
        else:
            return '\n'.join(self.bodylines)