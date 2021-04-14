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
    public Widget::Window
{
private:
    RefPtr<FilePicker::Navigation> _navigation;
    RefPtr<Archive> _archive;
    FilePicker::Dialog _dialog;

public:
    void set_archive(RefPtr<Archive> archive)
    {
        _archive = archive;
        render();
    }

    MainWindow(RefPtr<FilePicker::Navigation> navigation, RefPtr<Archive> archive)
        : Widget::Window(WINDOW_RESIZABLE), _navigation(navigation), _archive(archive)
    {
        icon(Graphic::Icon::get("folder-zip"));
        title("Archive Manager");
        size(Math::Vec2i(700, 500));

        render();
    }

    void render()
    {
        root()->clear_children();
        root()->layout(VFLOW(0));

        new Widget::TitleBar(root());

        auto browser = new Widget::Container(root());

        browser->flags(Widget::Component::FILL);

        if (!_archive)
        {
            browser->layout(STACK());
            auto load_button = new Widget::Button(browser, Widget::Button::FILLED, Graphic::Icon::get("folder-open"), "Load an archive file");
            load_button->on(Widget::Event::ACTION, [&](auto) {
                if (_dialog.show() == Widget::DialogResult::OK)
                {
                    set_archive(Archive::open(Path::parse(_dialog.selected_file().unwrap())));
                }
            });
        }
        else
        {
            browser->layout(VFLOW(0));

            auto toolbar = new Widget::Panel(browser);
            toolbar->layout(HFLOW(4));
            toolbar->insets(Insetsi(4, 4));

            new Widget::Button(toolbar, Widget::Button::TEXT, Graphic::Icon::get("archive-arrow-up"), "Extract All");

            new Widget::Separator(toolbar);

            auto load_button = new Widget::Button(toolbar, Widget::Button::TEXT, Graphic::Icon::get("folder-open"));
            load_button->on(Widget::Event::ACTION, [&](auto) {
                if (_dialog.show() == Widget::DialogResult::OK)
                {
                    set_archive(Archive::open(Path::parse(_dialog.selected_file().unwrap())));
                }
            });

            new FilePicker::ArchiveBrowser(browser, _navigation, _archive);
        }
    }
};
