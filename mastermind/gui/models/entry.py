import dearpygui.dearpygui as dpg


class Entry:
    def __init__(self, display_name, window_portal):
        self.display_name = display_name
        self.window_portal = window_portal(self.display_name)