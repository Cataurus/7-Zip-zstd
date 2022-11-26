Get-Item . | Set-Variable ROOT_PATH
Set-Variable VC 16.0
Set-Variable CLOUD "FALSE"

Set-Variable SUBSYS "5.02"
Set-Variable PLATFORM "x64"
Set-Variable DEBUG DEBUG

$BUILDDIR = Get-Item build

if ($BUILDDIR.Exists)
{
    Remove-Item $BUILDDIR -Recurse
}

mkdir build

Set-Location cpp

./build-clear.ps1
./build-it.ps1

Set-Location ..