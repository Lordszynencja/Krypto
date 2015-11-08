<script language="PHP">
$znaki[]="0";
$znaki[]="1";
$znaki[]="2";
$znaki[]="3";
$znaki[]="4";
$znaki[]="5";
$znaki[]="6";
$znaki[]="7";
$znaki[]="8";
$znaki[]="9";
$znaki[]="a";
$znaki[]="b";
$znaki[]="c";
$znaki[]="d";
$znaki[]="e";
$znaki[]="f";
$data="\x00\x1C\x14\xE8\x34\x88\x9A\x5A\x7F\x5C\xC3\x7C\x5B\x3C\xF0\x39\x5D\x41\x0B\xC6\x00\xD0\x0F\xCA\x89\xF3\x7F\x57\x35\xE8\x6C\xA1\x65\x6E\xC4\x78\x71\xDD\x7A\x74\x19\x1B\xAC\x78\xAD\x16\x17\x0A\xF0\x63\xF1\x4A\x94\x4A\x22\x73\x41\xDC\x4B\x56\x53\x44\xD6\x47\x46\xB8";
$td=mcrypt_module_open('arcfour','','stream','');
for ($z1=211;$z1<256;$z1++) {
for ($z2=107;$z2<256;$z2++) {
	echo("\rz1 $z1/256,z2 $z2/256  ");
for ($z3=0;$z3<256;$z3++) {
for ($z4=0;$z4<256;$z4++) {
	$key=$znaki[$z1%16].$znaki[$z1/16].$znaki[$z2%16].$znaki[$z2/16].$znaki[$z3%16].$znaki[$z3/16].$znaki[$z4%16].$znaki[$z4/16]."c793fdc5";
	mcrypt_generic_init($td,$key,'');
	$decrypted=mdecrypt_generic($td,$data);
	mcrypt_generic_deinit($td);
	$litery=0;
	$dlugosc=strlen($decrypted);
	for ($i=0;$i<$dlugosc;$i++) {
		if (ord($decrypted[$i])<32 || ord($decrypted[$i])>127) {
			$i=$dlugosc;
		} else if ((ord($decrypted[$i])>64 && ord($decrypted[$i])<91) || (ord($decrypted[$i])>96 && ord($decrypted[$i])<123) || ord($decrypted[$i])==32) {
			$litery++;
		}
	}
	if ($litery>$dlugosc-10) {
		echo("\r$decrypted\nfor key $key\n");
	}
}
}
}
}
mcrypt_module_close($td);
</script>