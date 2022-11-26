function LogError
{
    param([String]$Name)

    IF ($error.count -ne 0)
    {
        Write-Output "Error @ $Name" >> $ERRFILE
        Write-Output $error >> $ERRFILE
        $error.clear()
    }    
}

Set-Variable ROOT $PWD\7zip

mkdir $APPVEYOR_BUILD_FOLDER\build

if ($DEBUG -eq "DEBUG")
{
    Set-Variable OUTDIR $APPVEYOR_BUILD_FOLDER\build\bin-$PLATFORM-Debug
    Set-Variable ERRFILE $APPVEYOR_BUILD_FOLDER\build\bin-$PLATFORM-Debug.log
    Set-Variable LFLAGS "/SUBSYSTEM:WINDOWS,%SUBSYS% /Debug"
    Set-Variable CFLAGS "/Z7"
}
else 
{
    Set-Variable OUTDIR $APPVEYOR_BUILD_FOLDER\build\bin-$PLATFORM-Release
    Set-Variable ERRFILE $APPVEYOR_BUILD_FOLDER\build\bin-$PLATFORM-Release.log
    Set-Variable LFLAGS "/SUBSYSTEM:WINDOWS,%SUBSYS%"
    Set-Variable CFLAGS ""
}

if ($CLOUD -eq "FALSE")
{
    Set-Variable SOURCEDIR "o"
}
else
{
    Set-Variable SOURCEDIR $PLATFORM
}


Write-Output ****************************** >> $ERRFILE
Write-Output ** Uebersicht der Variabeln ** >> $ERRFILE
Write-Output ****************************** >> $ERRFILE
Write-Output ""
Get-Variable ROOT >> $ERRFILE
Get-Variable APPVEYOR_BUILD_FOLDER >> $ERRFILE
Get-Variable SOURCEDIR >> $ERRFILE
Get-Variable OUTDIR >> $ERRFILE
Get-Variable ERRFILE >> $ERRFILE
Get-Variable LFLAGS >> $ERRFILE
Get-Variable CFLAGS >> $ERRFILE

if (Test-Path OUTDIR){
    Get-Item -Path $OUTDIR | Remove-Item 
}
else {
    mkdir $OUTDIR
}


$error.clear()

Set-Location $ROOT\Bundles\Format7zExtract
nmake $OPTS
Copy-Item $SOURCEDIR\7zxa.dll $OUTDIR\7zxa.dll
LogError "7zxa.dll"

Set-Location $ROOT\Bundles\Format7z
nmake $OPTS
Copy-Item $SOURCEDIR\7za.dll $OUTDIR\7za.dll
LogError "7za.dll"

Set-Location $ROOT\Bundles\Format7zF
nmake $OPTS
Copy-Item $SOURCEDIR\7z.dll $OUTDIR\7z.dll
LogError "7z.dll" 

Set-Location $ROOT\UI\FileManager
nmake $OPTS
Copy-Item $SOURCEDIR\7zFM.exe $OUTDIR\7zFM.exe
LogError "7zFM.exe"

Set-Location $ROOT\UI\GUI
nmake $OPTS
Copy-Item $SOURCEDIR\7zG.exe $OUTDIR\7zG.exe
LogError "7zG.exe"

Set-Location $ROOT\UI\Explorer
nmake $OPTS
Copy-Item $SOURCEDIR\7-zip.dll $OUTDIR\7-zip.dll
LogError "7-zip.dll"

Set-Location $ROOT\Bundles\SFXWin
nmake $OPTS
Copy-Item $SOURCEDIR\7z.sfx $OUTDIR\7z.sfx
LogError "7z.sfx"

Set-Location $ROOT\Bundles\Codec_brotli
nmake $OPTS
Copy-Item $SOURCEDIR\brotli.dll $OUTDIR\brotli.dll
LogError "brotli.dll"

Set-Location $ROOT\Bundles\Codec_lizard
nmake $OPTS
Copy-Item $SOURCEDIR\lizard.dll $OUTDIR\lizard.dll
LogError "lizard.dll"

Set-Location $ROOT\Bundles\Codec_lz4
nmake $OPTS
Copy-Item $SOURCEDIR\lz4.dll $OUTDIR\lz4.dll
LogError "lz4.dll"

Set-Location $ROOT\Bundles\Codec_lz5
nmake $OPTS
Copy-Item $SOURCEDIR\lz5.dll $OUTDIR\lz5.dll
LogError "lz5.dll"

Set-Location $ROOT\Bundles\Codec_zstd
nmake $OPTS
Copy-Item $SOURCEDIR\zstd.dll $OUTDIR\zstd.dll
LogError "zstd.dll"

Set-Location $ROOT\Bundles\Codec_flzma2
nmake $OPTS
Copy-Item $SOURCEDIR\flzma2.dll $OUTDIR\flzma2.dll
LogError "flzma2.dll"

Set-Location $ROOT\..\..\C\Util\7zipInstall
nmake $OPTS
Copy-Item $SOURCEDIR\7zipInstall.exe $OUTDIR\Install.exe
LogError "7zipInstall.exe"

Set-Location $ROOT\..\..\C\Util\7zipUninstall
nmake $OPTS
Copy-Item $SOURCEDIR\7zipUninstall.exe $OUTDIR\Uninstall.exe
LogError "7zipUninstall.exe"

Set-Variable LFLAGS "/SUBSYSTEM:CONSOLE,$SUBSYS /Debug"

Set-Location $ROOT\UI\Console
nmake $OPTS
Copy-Item $SOURCEDIR\7z.exe $OUTDIR\7z.exe
LogError "7z.exe"

Set-Location $ROOT\Bundles\SFXCon
nmake $OPTS
Copy-Item $SOURCEDIR\7zCon.sfx $OUTDIR\7zCon.sfx
LogError "7zCon.sfx"

Set-Location $ROOT\Bundles\Alone
nmake $OPTS
Copy-Item $SOURCEDIR\7za.exe $OUTDIR\7za.exe
LogError "7za.exe"