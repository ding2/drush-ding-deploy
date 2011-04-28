<?php

$aliases['basic'] = array(
  'uri' => 'std.test.dev',
  'root' => '/tmp/ding-deploy-test/root',
  'profile-core-version' => '7.x',
  'profile-name' => 'basic',
  'profile-type' => 'file',
  'profile-url' => 'http://localhost/ding-deploy-test/basic.tar.gz',
  'env' => 'stg',
  'build-path' => '/tmp/ding-deploy-test/build',
  // 'post-updb' => array(
  //   'status',
  //   'bla',
  // ),
  // 'db-url' => 'pgsql://username:password@dbhost.com:port/databasename',
  // 'remote-host' => 'xen.dk',
  // 'remote-user' => 'xen',
  'path-aliases' => array(
    '%drush' => '/home/xen/bin/drush',
    '%drush-script' => '/home/xen/bin/drush/drush',
    // '%dump-dir' => '/path/to/dumps/',
    // '%files' => 'sites/mydrupalsite.com/files',
    // '%custom' => '/my/custom/path',
  ),
  // 'command-specific' => array (
  //   'sql-sync' => array (
  //     'no-cache' => TRUE,
  //   ),
  // ),
);

$aliases['basic2'] = array(
  'parent' => '@basic',
  'profile-name' => 'basic',
);
