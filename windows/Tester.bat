@echo off
setlocal
cd /d "%~dp0"
if not exist "markov_graph.exe" goto absent
echo GRAPHES ET CHAINES DE MARKOV
echo 1 - Petit graphe de 4 sommets
echo 2 - Graphe principal de 27 sommets
choice /c 12 /n /m "Choisir un exemple : "
if errorlevel 2 goto principal
markov_graph.exe data/exemple1
goto fin
:principal
markov_graph.exe data/proba
:fin
echo.
echo Les fichiers Mermaid generes restent dans ce dossier.
pause
exit /b
:absent
echo Extrayez tout le ZIP avant de lancer Tester.bat.
pause
exit /b 1
