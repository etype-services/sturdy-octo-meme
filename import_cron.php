<?php
// stop errors
$_SERVER['REMOTE_ADDR'] = '35.184.227.31';
$_SERVER['REQUEST_METHOD'] = 'GET';

// define static var
define('DRUPAL_ROOT', getcwd());

// include bootstrap
include_once('/mnt/data/web/public/drupal/includes/bootstrap.inc');

// initialize stuff
drupal_bootstrap(DRUPAL_BOOTSTRAP_FULL);

include '/mnt/data/web/public/drupal/sites/all/modules/etype_jailbirds/etype_jailbirds.import.inc';
etype_jailbirds_import();
?>
