import os
import wx
import re

from Puzzles import *

def natural_sort(l): 
    convert = lambda text: int(text) if text.isdigit() else text.lower() 
    alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ] 
    return sorted(l, key = alphanum_key)

app = wx.App()

class OpeningDialog(wx.Frame):
    def __init__(self):
        super(OpeningDialog, self).__init__(None, title='Position overview', size=(500, 150))

        panel = wx.Panel(self)

        vbox = wx.BoxSizer(wx.VERTICAL)

        hbox1 = wx.BoxSizer(wx.HORIZONTAL)
        label = wx.StaticText(panel, label='Folder to scan:')
        hbox1.Add(label)
        vbox.Add(hbox1, flag=wx.LEFT | wx.TOP, border=10)

        vbox.Add((-1, 5))
        
        hbox2 = wx.BoxSizer(wx.HORIZONTAL)
        self.directory = wx.TextCtrl(panel)
        hbox2.Add(self.directory, proportion=1, flag=wx.RIGHT, border=8)
        btn_browse = wx.Button(panel, label="Browse...")
        hbox2.Add(btn_browse)
        vbox.Add(hbox2, proportion=1, flag=wx.EXPAND|wx.LEFT|wx.RIGHT, border=10)

        vbox.Add((-1, 5))
        
        hbox5 = wx.BoxSizer(wx.HORIZONTAL)
        btn_OK = wx.Button(panel, label='Ok', size=(70, 30))
        hbox5.Add(btn_OK)
        btn_Close = wx.Button(panel, label='Close', size=(70, 30))
        hbox5.Add(btn_Close, flag=wx.LEFT|wx.BOTTOM, border=5)
        vbox.Add(hbox5, flag=wx.ALIGN_RIGHT|wx.RIGHT, border=10)

        panel.SetSizer(vbox)

        self.Centre()

        self.Bind(wx.EVT_BUTTON, self.OnButtonClicked_Browse, id=btn_browse.GetId())
        self.Bind(wx.EVT_BUTTON, self.OnButtonClicked_OK, id=btn_OK.GetId())
        self.Bind(wx.EVT_BUTTON, self.OnButtonClicked_Close, id=btn_Close.GetId())

    def OnButtonClicked_Browse(self, e):
        dlg_dir = wx.DirDialog(None, "Choose input directory", "", wx.DD_DEFAULT_STYLE)
        if dlg_dir.ShowModal() == wx.ID_OK:
            self.directory.Clear()
            self.directory.SetValue(dlg_dir.GetPath())
        e.Skip()

    def OnButtonClicked_OK(self, e):
        overview = MainDialog(self.directory.GetValue())
        self.Close()
        overview.Show()

    def OnButtonClicked_Close(self, e):
        self.Close()

class MainDialog(wx.Frame):
    def __init__(self, directory):
        super(MainDialog, self).__init__(None, title='Position overview', size=(400, 800))
        
        filenames = natural_sort([filename for filename in os.listdir(directory) if filename.endswith(".pos")])

        panel = wx.PyScrolledWindow(self, -1)

        vbox = wx.BoxSizer(wx.VERTICAL)
        fgs = wx.FlexGridSizer(len(filenames), 3, 9, 25)

        for filename in filenames:
            label = wx.StaticText(panel, label=filename)
            gauge = wx.Gauge(panel, range=100, style=wx.GA_HORIZONTAL)
            #gauge.Value = PercentageOfSolvedPositions(directory + '\\' + filename)
            percentage = wx.StaticText(panel, label = str(gauge.Value) + '%')

            fgs.AddMany([(label, 1, wx.EXPAND), (gauge), (percentage)])

        fgs.AddGrowableCol(0, 1)
        vbox.Add(fgs, proportion=1, flag=wx.ALL|wx.EXPAND, border=10)
        panel.SetSizer(vbox)
        panel.SetScrollbar(0,0,0,0)
        panel.SetScrollRate(1, 1)

        self.Centre()

def PercentageOfSolvedPositions(filename):
    sum = 0
    counter = 0
    for puzzle in Deserialize_Puzzles(filename):
        if puzzle.IsSolved():
            sum += 1
        counter += 1
    if sum > 0:
        return counter * 100 / sum
    else:
        return 0


def main():
    app = wx.App()
    opening_dialog = OpeningDialog()
    opening_dialog.Show()
    app.MainLoop()


if __name__ == '__main__':
    main()
