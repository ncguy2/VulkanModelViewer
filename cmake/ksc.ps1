param(
    [string]$kscHome
)


$ksc="${kscHome}bin\kaitai-struct-compiler.bat"
Write-Output "KSC: $ksc"

Write-Output ". $ksc $args"
. "$ksc" $args
