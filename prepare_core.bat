@ECHO OFF
IF NOT EXIST tmk_core (
	git remote add -f core https://github.com/tmk/tmk_core
	git subtree add -P tmk_core core master --squash
) ELSE (
	echo "tmk_core initialized already"
)

REM Remove things from tmk_core that are huge AND not a part of the core.

cd tmk_core/tool/mbed
CALL :GITIGNORE mbed-sdk

cd ../../protocol/lufa
CALL :GITIGNORE LUFA-git

cd ../
CALL :GITIGNORE usb_hid

cd ../..

IF NOT EXIST hidapi (
	git remote add -f hidapi https://github.com/signal11/hidapi
	git subtree add -P hidapi hidapi master --squash
) ELSE (
	echo "hidapi initialized already"
)

EXIT

:GITIGNORE
echo marking %1
git ls-files %1 | grep " " | xargs -L 1 -i git update-index --assume-unchanged "{}"
git ls-files %1 | grep -v " " | xargs -L 100 git update-index --assume-unchanged
echo removing %1
rmdir /s/q %1
echo Done!
EXIT /B