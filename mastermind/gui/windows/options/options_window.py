from models.window import Window
import dearpygui.dearpygui as dpg


class OptionsWindow(Window):
    def window_data(self):
        dpg.add_text("Application-wide options go here")