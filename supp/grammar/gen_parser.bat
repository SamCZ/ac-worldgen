@echo off
java -jar antlr-4.7.2-complete.jar -o ..\..\src\woglac\parser -Dlanguage=Cpp Woglac.g4

	REM Without this, you'd get compile errors on MSVC2019
REM The following replaces all 'u8"' with '"' in the file.
powershell -Command "(Get-Content ..\..\src\woglac\parser\WoglacLexer.cpp) -replace 'u8\"', '\"' | Set-Content ..\..\src\woglac\parser\WoglacLexer.cpp"
pause