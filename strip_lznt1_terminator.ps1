$p = Join-Path $PSScriptRoot 'bin\profile_internal.bin'
$b = [IO.File]::ReadAllBytes($p)

# walk LZNT1 chunk headers; body starts after 4-byte fourcc + 32-byte hash
$i = 36
while ($i + 2 -le $b.Length) {
    $h = $b[$i] -bor ($b[$i + 1] -shl 8)
    $sz = ($h -band 0xFFF) + 1
    if ($i + 2 + $sz -gt $b.Length) { break }
    $i += 2 + $sz
}

$left = $b.Length - $i
if ($left -eq 2 -and $b[$i] -eq 0 -and $b[$i + 1] -eq 0) {
    $nb = New-Object byte[] $i
    [Array]::Copy($b, 0, $nb, 0, $i)
    [IO.File]::WriteAllBytes($p, $nb)
    Write-Host 'profile_internal.bin: stripped 2-byte LZNT1 terminator'
} elseif ($left -eq 0) {
    Write-Host 'profile_internal.bin: ok'
} else {
    throw "profile_internal.bin: malformed stream (leftover $left bytes at offset $i)"
}
