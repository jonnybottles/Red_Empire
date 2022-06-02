from abc import ABC, abstractmethod
import dearpygui.dearpygui as dpg


class Window(ABC):
    def __init__(self, tag):
        self.button_width = 300
        self.tag = tag

    def load(self):
        with dpg.child_window(tag=self.tag, show=False, parent="main_group", menubar=True, autosize_x=True,
                          autosize_y=True):
            with dpg.menu_bar():
                dpg.add_text(self.tag)
            self.window_data()

    @abstractmethod
    def window_data(self):
        raise NotImplementedError

    def open(self):
        dpg.configure_item(self.tag, show=True)

    def close(self):
        dpg.configure_item(self.tag, show=False)