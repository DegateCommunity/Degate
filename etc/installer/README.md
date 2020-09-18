# Degate installer

The Degate installer uses the Qt Installer Framework that you can download here : http://download.qt.io/official_releases/qt-installer-framework/.
It works on Windows, Linux and Mac.

## Process

- Update the Degate version and release date in the 'VERSION' file.
- Build the project.
- On Windows, run 'windeployqt.exe' to automatically get all Qt stuff near the executable.
- On Linux, you can use https://github.com/probonopd/linuxdeployqt (the best is to use the oldest Ubuntu LTS to build Degate and use linuxdeployqt).
- Copy all freshly built files to the 'packages/Degate/data' folder.
- Run this command (in the 'installer' folder): 
	- For Windows: 
	```console
    > path_to_binarycreator.exe -c config\config.xml -p packages "Degate installer.exe"
    ```
	- Others:
	```console
    > path_to_binarycreator -c config/config.xml -p packages Degate
    ```

## License

If the license change, copy the new license file to 'packages/Degate/meta/LICENSE.TXT'.

## Localization

See the 'packages/Degate/meta' folder for translations and the 'packages/Degate/meta/package.xml' file.

To update/add new languages run:
```console
> lupdate installscript.qs DesktopShortcutCheckBoxForm.ui ReadMeCheckBoxForm.ui StartMenuShortcutCheckBoxForm.ui -ts languages/<language>.ts
```
	
To generate the translation file run (in the 'languages' directory):
```console
> lrelease <language>.ts
```
	
Replace <language> with the two letter lowercase code for the language you want to translate to.
	
Languages:

- English 'en' (100%),
- French 'fr' (100%).
- German 'de' (0%),
- Russian 'ru' (0%),
- Spanish 'es' (0%),
- Italian 'it' (0%),
- Korean 'ko' (0%).
