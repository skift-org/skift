#pragma once

#include <libfile/Archive.h>
#include <libwidget/Container.h>
#include <libwidget/Panel.h>
#include <libwidget/TitleBar.h>
#include <libwidget/Window.h>

#include <libfilepicker/FilePicker.h>
#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/ArchiveBrowser.h>
#include <libfilepicker/widgets/JumpList.h>
#include <libfilepicker/widgets/ToolBar.h>

class MainWindow :
    public Window
{
private:
    RefPtr<filepicker::Navigation> _navigation;
    RefPtr<Archive> _archive;
    filepicker::Dialog _dialog;

public:
    void set_archive(RefPtr<Archive> archive)
    {
        _archive = archive;
        render();
    }

    MainWindow(RefPtr<filepicker::Navigation> navigation, RefPtr<Archive> archive)
        : Window(WINDOW_RESIZABLE), _navigation(navigation), _archive(archive)
    {
        icon(Icon::get("folder-zip"));
        title("Archive Manager");
        size(Vec2i(700, 500));

        render();
    }

    void render()
    {
        root()->clear_children();
        root()->layout(VFLOW(0));

        new TitleBar(root());

        auto browser = new Container(root());

        browser->flags(Widget::FILL);

        if (!_archive)
        {
            browser->layout(STACK());
            auto load_button = new Button(browser, Button::FILLED, Icon::get("folder-open"), "Load an archive file");
            load_button->on(Event::ACTION, [&](auto) {
                if (_dialog.show() == DialogResult::OK)
                {
                    set_archive(Archive::open(Path::parse(*_dialog.selected_file())));
                }
            });
        }
        else
        {
            browser->layout(VFLOW(0));

            auto toolbar = new Panel(browser);
            toolbar->layout(HFLOW(4));
            toolbar->insets(Insetsi(4, 4));
            toolbar->max_height(38);
            toolbar->min_height(38);

            new Button(toolbar, Button::TEXT, Icon::get("archive-arrow-up"), "Extract All");

            new Separator(toolbar);

            auto load_button = new Button(toolbar, Button::TEXT, Icon::get("folder-open"));
            load_button->on(Event::ACTION, [&](auto) {
                if (_dialog.show() == DialogResult::OK)
                {
                    set_archive(Archive::open(Path::parse(*_dialog.selected_file())));
                }
            });

            new filepicker::ArchiveBrowser(browser, _navigation, _archive);
        }
    }
};
