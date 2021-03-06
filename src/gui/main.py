import os

from kivy.app import App
from kivy.uix.floatlayout import FloatLayout
from kivy.properties import ObjectProperty
from kivy.properties import StringProperty
from kivy.uix.popup import Popup
from kivy.uix.boxlayout import BoxLayout
from kivy.uix.behaviors import DragBehavior
from kivy.uix.widget import Widget
from kivy.uix.treeview import TreeView

from genicam import *

class LoadDialog(FloatLayout):
    load = ObjectProperty(None)
    cancel = ObjectProperty(None)


class SaveDialog(FloatLayout):
    save = ObjectProperty(None)
    text_input = ObjectProperty(None)
    cancel = ObjectProperty(None)

class DragWidget(DragBehavior, Widget):
    pass

class FeatureTreeView(DragBehavior, TreeView):
    pass

class RootLayout(BoxLayout):
    image = ObjectProperty(None)
    loadfile = ObjectProperty(None)
    savefile = ObjectProperty(None)
    text_input = ObjectProperty(None)

    devices = []
    nodemaps = []

    def dismiss_popup(self):
        self._popup.dismiss()

    def show_load(self):
        content = LoadDialog(load=self.load, cancel=self.dismiss_popup)
        self._popup = Popup(title="Load file", content=content,
                            size_hint=(0.9, 0.9))
        self._popup.open()

    def show_save(self):
        content = SaveDialog(save=self.save, cancel=self.dismiss_popup)
        self._popup = Popup(title="Save file", content=content,
                            size_hint=(0.9, 0.9))
        self._popup.open()

    def load(self, path, filename):
        self.image.source = os.path.join(path, filename[0])

        self.dismiss_popup()

    def save(self, path, filename):
        self.dismiss_popup()

    def connect(self):
        systems = System_getSystems()
        print('systemsize = ', len(systems))
        for system in systems:
            try:
                system.open()
                interfs = system.getInterfaces()
                for interf in interfs:
                    interf.open()
                    for dev in interf.getDevices():
                        dev.open(Device.CONTROL)
                        self.devices.append(dev)
            except Exception as e:
                print(e)


class LyucamApp(App):
    pass

if __name__ == '__main__':
    LyucamApp().run()
