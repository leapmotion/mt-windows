:: Ensure that this is run within a VS command prompt, to ensure that the makecert and pvk2pfx
:: tools are available.
makecert -r -pe -n "CN=OcuSpecTest" -sv OcuSpec.pvk OcuSpec.cer
erase *.pfx
pvk2pfx -pvk OcuSpec.pvk -spc OcuSpec.cer -pfx OcuSpec.pfx -po "password"