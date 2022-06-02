#!/usr/bin/python3

# Requires DearPyGui
# -- pip install dearpygui

import dearpygui.dearpygui as dpg

from models.entry import Entry
from models.sidebar import Sidebar
from windows.listener.listener_window import ListenerWindow
from windows.options.options_window import OptionsWindow

class RunApp:
    def __init__(self):
        self.app_title = "RED ALERT"
        self.app_width = 1500
        self.app_height = 800
        self.app_min_width = 1500
        self.app_min_height = 800
        self.app_starting_x = 0
        self.app_starting_y = 0
        self.font_size = 16
        self.app_font = "fonts/MonoLisa-Regular.ttf"

        self.start_gui()

    def start_gui(self):
        # Creates gui context
        dpg.create_context()

        # Registers various fonts, textures, and theme data.
        self._do_registries()

        # Add draws to gui
        self.gui_data()

        # Begins gui loop and cleans up when done
        self._finalize_gui()

    def _do_registries(self):
        with dpg.font_registry():
            main_font = dpg.add_font(self.app_font, self.font_size)
        dpg.bind_font(main_font)

    def gui_data(self):
        with dpg.window(tag="main_window") as main_window:
            # Create sidebar
            # Tag currently must be main_group for windows to properly work
            with dpg.group(horizontal=True, tag="main_group"):
                # Create sidebar
                sb = Sidebar(tag="main_sb")

                # Create entries
                win_listener = Entry("View Listeners", ListenerWindow)
                win_options = Entry("Options", OptionsWindow)

                # Assign Entries
                sb.add_entry(win_listener)

                # Set options window
                sb.set_options(win_options)

                # Spawn custom sidebar
                sb.spawn()

    def _finalize_gui(self):
        dpg.set_primary_window("main_window", True)
        dpg.create_viewport(title=self.app_title, width=self.app_width,
                        height=self.app_height, x_pos=self.app_starting_x, y_pos=self.app_starting_y,
                        min_width=self.app_min_width, min_height=self.app_min_height)
        dpg.setup_dearpygui()
        dpg.show_viewport()
        dpg.start_dearpygui()
        dpg.destroy_context()


if __name__ == "__main__":
    RunApp()