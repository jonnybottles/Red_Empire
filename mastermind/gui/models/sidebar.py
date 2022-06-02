import dearpygui.dearpygui as dpg


class Sidebar:
    def __init__(self, tag):
        # Determines custom attributes
        self.width = 300
        self.primary_btn_height = 50
        self.primary_btn_width = 300
        self.split_btn_width = 145
        self.tag = tag

        # Sidebar title toggle
        self.has_title = True
        self.has_options = True

        # Sidebar title text
        self.title = "Home"

        # Enables sidebar to open and close.
        self.is_shutter = True

        # List of entries that are within sidebar
        self.option_entry = None
        self.entries = []

        # Current active entry
        self.active_entry = None

    def set_options(self, entry):
        self.option_entry = entry

    def update_entry_display(self, sender, app, user):
        # Close all menus
        for entry in self.entries:
            entry.window_portal.close()

        # Check for option menu
        if self.option_entry:
            self.option_entry.window_portal.close()

        self.active_entry = user

        # Open requested menu
        user.window_portal.open()

    def spawn(self):
        # Create sidebar recovery
        dpg.add_button(label=">", show=False, tag=f"{self.tag}_recover",
                       callback=lambda: [dpg.configure_item(self.tag, show=True),
                                         dpg.configure_item(f"{self.tag}_recover", show=False)])

        # Create window
        with dpg.child_window(autosize_y=True, width=self.width, border=False, tag=self.tag, menubar=self.has_title):
            # Setup menu bar
            with dpg.menu_bar():
                if self.is_shutter:
                    dpg.add_button(label="<", callback=lambda: [dpg.configure_item(self.tag, show=False),
                                                                dpg.configure_item(f"{self.tag}_recover", show=True)])
                if self.has_title:
                    dpg.add_text(self.title, indent=130)

            # Draw window buttons
            for entry in self.entries:
                # Loads window gui data
                entry.window_portal.load()

                # Add button for each entry
                dpg.add_button(label=entry.display_name, width=-1, height=self.primary_btn_height, callback=self.update_entry_display,
                               user_data=entry)

            if self.has_options:
                dpg.add_spacer(height=15)
                dpg.add_separator()
                dpg.add_spacer(height=15)

                with dpg.group(horizontal=True):
                    dpg.add_button(label="OPTIONS", height=self.primary_btn_height, width=self.split_btn_width, callback=self.update_entry_display,
                                   user_data=self.option_entry)
                    dpg.add_button(label="FULLSCREEN", height=self.primary_btn_height, width=self.split_btn_width,
                                   callback=lambda: dpg.toggle_viewport_fullscreen())
        # Load options
        if self.option_entry:
            self.option_entry.window_portal.load()

        # Launch default entry
        if self.entries:
            self.entries[0].window_portal.open()

    def add_entry(self, entry):
        self.entries.append(entry)