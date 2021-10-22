#pragma once

#include <libfile/Archive.h>

#include <libwidget/Window.h>

#include <libfilepicker/FilePicker.h>
#include <libfilepicker/model/Navigation.h>
#include <libfilepicker/widgets/ArchiveBrowser.h>
#include <libfilepicker/widgets/JumpList.h>
#include <libfilepicker/widgets/ToolBar.h>

struct MainWindow :
    public Widget::Window
{
private:
    Ref<FilePicker::Navigation> _navigation;
    Ref<Archive> _archive;
    FilePicker::Dialog _dialog;

public:
    void set_archive(Ref<Archive> archive)
    {
        _archive = archive;
        should_rebuild();
    }

    MainWindow(Ref<FilePicker::Navigation> navigation, Ref<Archive> archive)
        : Widget::Window(WINDOW_RESIZABLE),
          _navigation(navigation),
          _archive(archive)
    {
    }

    virtual Ref<Widget::Element> build()
    {
        using namespace Widget;

        // clang-format off

        if (_archive == nullptr)
        {
            return min_size(
                {700, 500},
                vflow({
                    titlebar(Graphic::Icon::get("folder-zip"), "Archive Manager"),
                    fill(
                        stack(
                            filled_button(Graphic::Icon::get("folder-open"), "Load an archive file", [&] {
                                if (_dialog.show() == DialogResult::OK)
                                {
                                    set_archive(Archive::open(IO::Path::parse(_dialog.selected_file().unwrap())));
                                }
                            })
                        )
                    )
                })
            );
        }
        else
        {
            return min_size(
                {700, 500},
                vflow({
                    titlebar(Graphic::Icon::get("folder-zip"), IO::format("Archive Manager - {}", _archive->get_path().basename())),
                    toolbar({
                        basic_button(Graphic::Icon::get("archive-arrow-up"), "Extract All"),
                        separator(),
                        basic_button(Graphic::Icon::get("folder-open"), [&] {
                            if (_dialog.show() == Widget::DialogResult::OK)
                            {
                                set_archive(Archive::open(IO::Path::parse(_dialog.selected_file().unwrap())));
                            }
                        })
                    }),
                    make<FilePicker::ArchiveBrowser>(_navigation, _archive)
                })
            );
        }

        // clang-format on
    }
};
