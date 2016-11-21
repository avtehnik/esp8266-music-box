<?php

unlink(__DIR__ . '/build/soundbox.appcache');
if (file_exists(__DIR__ . '/../out/firmware/spiff_rom.bin')) {
    unlink(__DIR__ . '/../out/firmware/spiff_rom.bin');
}

copy(__DIR__ . "/bootstrap-3.3.7-dist/css/bootstrap.min.css", __DIR__ . "/build/bootstrap.css");
copy(__DIR__ . "/style.css", __DIR__ . "/build/style.css");
copy(__DIR__ . "/bootstrap-3.3.7-dist/js/bootstrap.min.js", __DIR__ . "/build/bootstrap.js");
copy(__DIR__ . "/jquery.js", __DIR__ . "/build/jquery.js");
copy(__DIR__ . "/index.js", __DIR__ . "/build/index.js");
copy(__DIR__ . "/index.html", __DIR__ . "/build/index.html");


$lines = [];
$lines[] = 'CACHE MANIFEST';
$lines[] = '#' . date('d.m.y h:i:s');

foreach (scandir(__DIR__ . '/build') as $item) {
    if (is_file($item)) {
        $lines[] = '/' . $item;
    }
}


$dir = __DIR__;

`gzip -f $dir/build/bootstrap.css`;
`gzip -f  $dir/build/style.css`;
`gzip -f  $dir/build/bootstrap.js`;
`gzip -f  $dir/build/jquery.js`;
`gzip -f  $dir/build/index.js`;


file_put_contents(__DIR__ . '/build/soundbox.appcache', implode("\n", $lines));

