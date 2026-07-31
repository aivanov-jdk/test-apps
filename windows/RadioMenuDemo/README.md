# Radio Menu Demo

This is a sample Win32 app for
[JDK-8348760](https://bugs.openjdk.org/browse/JDK-8348760
"RadioButton is not shown if JRadioButtonMenuItem is rendered with ImageIcon in WindowsLookAndFeel").
It creates a simple menu with icons.

The first version of the app in `rbm.cpp` is implemented by
Prasanta Sadhukhan, then Phil Race copied the source to
`rbma.cpp` and modified it so that the app renders a custom
radio bullet bitmap.

Both apps could be easily built in Developer shell for
Visual Studio:

```shell
cl -DUNICODE rbm.cpp user32.lib gdi32.lib
cl -DUNICODE rbma.cpp user32.lib gdi32.lib
```
