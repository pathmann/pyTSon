from PythonQt.QtGui import (QDialog, QVBoxLayout, QHBoxLayout, QLabel,
                            QLineEdit, QDialogButtonBox)


class MultiInputDialog(QDialog):
    def __init__(self, title, label1, label2, parent=None):
        super().__init__(parent)
        self.setWindowTitle(title)
        self.lay = QVBoxLayout(self)

        self.lay1 = QHBoxLayout()
        self.label1 = QLabel(label1, self)
        self.lay1.addWidget(self.label1)
        self.input1 = QLineEdit(self)
        self.lay1.addWidget(self.input1)
        self.lay.addLayout(self.lay1)

        self.lay2 = QHBoxLayout()
        self.label2 = QLabel(label2, self)
        self.lay2.addWidget(self.label2)
        self.input2 = QLineEdit(self)
        self.lay2.addWidget(self.input2)
        self.lay.addLayout(self.lay2)

        bbox = QDialogButtonBox(QDialogButtonBox.Ok | QDialogButtonBox.Cancel,
                                self)
        bbox.connect("accepted()", self.accept)
        bbox.connect("rejected()", self.reject)
        self.lay.addWidget(bbox)

    def cleanup(self):
        self.lay1.delete()
        self.lay2.delete()

    @staticmethod
    def getTexts(title, label1, label2, text1="", text2="", parent=None):
        dlg = MultiInputDialog(title, label1, label2, parent)

        dlg.label1.setText(label1)
        dlg.input1.setText(text1)
        dlg.label2.setText(label2)
        dlg.input2.setText(text2)

        if dlg.exec_() == QDialog.Accepted:
            ret1 = dlg.input1.text
            ret2 = dlg.input2.text

            dlg.cleanup()
            dlg.delete()

            return (True, ret1, ret2)
        else:
            return (False, "", "")
