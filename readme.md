# Karm

A C++ framework for building rich interactive applications.

## Features

### Reactive UI

```c++
ExitCode entryPoint(CliArgs const &) {
    Ui app{[] {
        window([&] {
            auto state = useState(0);

            label("You, clicked {} times", *state);

            button("Click me!", [&](Event const &) {
                state.update([](auto &s) {
                    s++;
                });
            });
        });
    }};

    return app.run();
}
```

### Advance Cli

```c++
ExitCode entryPoint(CliArgs const &args) {
  Cli app{[]{
    auto exclamative = withParam('e', "exclamative", false);
    auto name = withArg("name", "world");

    effect([&]{
      Fmt::print("Hello {:case-title} {}!", name, exclamative ? "!" : ".");
    });
  }}

  return app.eval(args);
}
```

## Try it out

```sh
$ git clone https://github.com/skiftOS/karm.git
$ cd karm
$ ./sk.sh run counter-app
```
