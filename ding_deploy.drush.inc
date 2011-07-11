<?php

/**
 * @file
 *   Ding deployment functions.
 */

define('DING_DEPLOY_GIT_REPO', 'https://github.com/dingproject/drush-ding-deploy.git');

define('BOOTSTRAP_MAKE','api = 2
core = @core-version

projects[] = drupal

projects[@name][type] = "profile"
projects[@name][download][type] = "@type"
projects[@name][download][url] = "@url"
');

/**
 * Implements hook_drush_command().
 */
function ding_deploy_drush_command() {
  $items = array();

  $items['ding-deploy-install'] = array(
    'description' => "Install Ding Deploy on the server.",
    'arguments' => array(
      'site-alias' => 'The site alias to install to',
    ),
    'examples' => array(
      'drush ding-deploy-install @staging' => 'Install on the @staging server.',
    ),
    'aliases' => array('ddinstall'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['ding-deploy-update'] = array(
    'description' => "Update Ding Deploy on the server.",
    'arguments' => array(
      'site-alias' => 'The site alias to update',
    ),
    'examples' => array(
      'drush ding-deploy-update @staging' => 'Update on the @staging server.',
    ),
    'aliases' => array('ddupdate'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['ding-deploy-setup'] = array(
    'description' => "Set up server for future deployments of site.",
    'options' => array(
      'profile-name' => 'The name of the profile.',
      'build-path' => 'Full path to build directory',
      'no-core' => "Don't install Drupal core if not found",
      'no-symlink' => "Don't create a symlink in Drupal profile directory",
    ),
    'examples' => array(
      'drush @staging --build-path=/home/deploy/build/sitename ding-deploy-setup' => 'Set up the @staging server.',
    ),
    'aliases' => array('ddsetup'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );


  $items['ding-deploy'] = array(
    'callback' => 'drush_ding_deploy',
    'description' => "Deploy to an environment.",
    'options' => array(
      'profile-name' => 'The name of the profile.',
      'env' => 'The environment that is build, for instance: "staging"',
      'build-path' => 'Full path to build directory',
      'no-offline' => "Don't set site offline",
      'no-dump' => "Don't dump database",
      'no-updb' => "Don't run database updates",
      'no-post-updb' => "Don't run post update commands",
      'code-only' => "Alias for --no-offline --no-dump --no-updb --no-post-updb. Useful for first deployment and when you know there's no need for a full deployment",
      'working-copy' => 'Create checkouts when possible',
    ),
    'examples' => array(
      'drush @staging --build-path=/home/deploy/build ding-deploy' => 'Deploy to the staging server.',
    ),
    'aliases' => array('ddeploy'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['ding-deploy-build'] = array(
    'description' => "Build profile.",
    'arguments' => array(
      'path' => 'The path to build to. Optional, defaults to site root',
    ),
    'options' => array(
      'clean' => 'Delete any existing profile before building',
      'profile-core-version' => '',
      'profile-name' => 'Name of the profile',
      'profile-type' => 'Fetch method. Same as type in drush make files. Defaults to git',
      'profile-url' => 'The url to fetch the profile from',
      'profile-branch' => 'Checkout branch instead of master',
      'profile-tag' => 'Checkout specific tag',
      'working-copy' => 'Create checkouts when possible',
    ),
    'examples' => array(
      'drush @dev ding-build /var/www/drupal-7.x' => 'Build ding in the specified dir.',
    ),
    'aliases' => array('ddbuild'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  $items['ding-deploy-test'] = array(
    'description' => "Run some tests.",
    'aliases' => array('ddtest'),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH, // No bootstrap at all.
  );

  return $items;

}

/**
 * Implements of hook_drush_help().
 */
function ding_deploy_drush_help($section) {
  switch ($section) {
    case 'meta:ding_deploy:title':
      return dt('Ding Deploy');
    case 'meta:ding_deploy:summary':
      return dt('Deploy ding to servers.');
    case 'drush:ding-deploy-install':
      return dt("Install Ding Deploy on server.

This will install Ding Deploy in the deployment users ~/.drush folder. Must be run locally.");
    case 'drush:ding-deploy-update':
      return dt("Update Ding Deploy on server.

This will attempt to update Ding Deploy on the server using git pull. Must be run locally.");
    case 'drush:ding-deploy-setup':
      return dt("Set up a site for future deployments.

Will install Drupal and set up proper symlinks to the build directory.
");
    case 'drush:ding-deploy':
      return dt("Deploy site.

Builds the site and moves the symlinks if the build was successfull.
");
    case 'drush:ding-deploy-build':
      return dt("Build site.

Builds the site. Mostly useful for developers, but is used internally by ding-deploy.
");
    case 'error:NO_ENV':
      return dt("No environment specified.");
    case 'error:NO_BUILD_PATH':
      return dt("No build path specified.");
    case 'error:NO_ROOT_PATH':
      return dt("No Drupal root specified.");
    case 'error:NO_REMOTE_USER':
      return dt("No remote user specified.");
    case 'error:NO_REMOTE_HOST':
      return dt("No remote host specified.");
    case 'error:INSTALL_ERROR':
      return dt("There was an error installing.");
    case 'error:INSTALL_UPDATE':
      return dt("There was an error updating.");
    case 'error:ERROR_CREATING_BUILD_PATH':
      return dt("Could not create build directory.");
    case 'error:BUILD_PATH_DOES_NOT_EXIST':
      return dt("Build path doesn't exist.");
    case 'error:MAKE_FAILED':
      return dt("Drush make failed on bootstrap.make.");
    case 'error:BUILD_FAILED':
      return dt("Error building, there's not a symlink in the way of the profile?");
    case 'error:DRUPAL_DOWNLOAD_FAILED':
      return dt("Installing Drupal failed.");
    case 'error:NO_DBDUMP_FILE':
      return dt("No database dump file to restore.");
    case 'error:NO_PROFILE_CORE_VERSION':
      return dt("No profile core version supplied.");
    case 'error:NO_PATH_OR_ROOT':
      return dt("Root dir needs to be specified if omitting path.");
    case 'error:NO_PROFILE_NAME':
      return dt("No profile name supplied.");
    case 'error:NO_PROFILE_URL':
      return dt("No profile url supplied.");
    case 'error:PATH_EXISTS':
      return dt("Path already exists.");
  }
}

/**
 * Drush ding-deploy-install validate callback.
 */
function drush_ding_deploy_install_validate($alias) {
  $options = array();
  // Expand alias.
  $settings = drush_sitealias_evaluate_path($alias, $options);

  foreach( $settings as $key => $v ) {
    drush_set_option( $key, $v );
  }

  if (!drush_get_option('remote-user', NULL)) {
    return drush_set_error('NO_REMOTE_USER');
  }
  if (!drush_get_option('remote-host', NULL)) {
    return drush_set_error('NO_REMOTE_HOST');
  }
}

/**
 * Drush ding-deploy-install command callback.
 */
function drush_ding_deploy_install() {
  $username = drush_get_option('remote-user', NULL);
  $hostname = drush_get_option('remote-host', NULL);
  $ssh_options = drush_get_option('ssh-options', "-o PasswordAuthentication=no");
  $cmd = 'mkdir -p .drush; cd .drush; git clone ' . DING_DEPLOY_GIT_REPO;
  $cmd = "ssh " . $ssh_options . " " . escapeshellarg($username) . "@" . escapeshellarg($hostname) . " " . escapeshellarg($cmd);
  if (!drush_shell_exec($cmd)) {
    foreach (drush_shell_exec_output() as $line) {
      drush_log($line, 'warning');
    }
    return drush_set_error('INSTALL_ERROR');
  }
  drush_print("Ding Deploy installed.");
}

/**
 * Drush ding-deploy-update validate callback.
 */
function drush_ding_deploy_update_validate($alias) {
  $options = array();
  // Expand alias.
  $settings = drush_sitealias_evaluate_path($alias, $options);

  if (!drush_get_option('remote-user', NULL)) {
    return drush_set_error('NO_REMOTE_USER');
  }
  if (!drush_get_option('remote-host', NULL)) {
    return drush_set_error('NO_REMOTE_HOST');
  }
}

/**
 * Drush ding-deploy-update command callback.
 */
function drush_ding_deploy_update() {
  $username = drush_get_option('remote-user', NULL);
  $hostname = drush_get_option('remote-host', NULL);
  $ssh_options = drush_get_option('ssh-options', "-o PasswordAuthentication=no");
  $cmd = 'cd .drush/drush-ding-deploy; git pull';
  $cmd = "ssh " . $ssh_options . " " . escapeshellarg($username) . "@" . escapeshellarg($hostname) . " " . escapeshellarg($cmd);
  if (!drush_shell_exec($cmd)) {
    foreach (drush_shell_exec_output() as $line) {
      drush_log($line, 'warning');
    }
    return drush_set_error('UPDATE_ERROR');
  }
  drush_print("Ding Deploy updated.");
}

/**
 * Drush ding-deploy-setup validate callback.
 */
function drush_ding_deploy_setup_validate() {
  if (!drush_get_option('profile-name', NULL)) {
    return drush_set_error('NO_PROFILE_NAME');
  }
  if (!drush_get_option('env', NULL)) {
    return drush_set_error('NO_ENV');
  }
  if (!drush_get_option('build-path', NULL)) {
    return drush_set_error('NO_BUILD_PATH');
  }
  if (!drush_get_option('root', NULL)) {
    return drush_set_error('NO_ROOT_PATH');
  }
}

/**
 * Drush ding-deploy-setup command callback.
 */
function drush_ding_deploy_setup() {
  $root = drush_get_option('root', NULL);
  $profile = drush_get_option('profile-name', NULL);
  $build_path = drush_get_option('build-path');
  $env = drush_get_option('env');
  if (!file_exists($build_path)) {
    if (!drush_mkdir($build_path)) {
      return drush_set_error('ERROR_CREATING_BUILD_PATH');
    }
    drush_log('Created build dir.');
  }

  if (!drush_get_option('no-core', FALSE)) {
    if (!file_exists(rtrim($root, '/') . '/profiles')) {
      if (file_exists($root)) {
        // Directory already exist.
        if (sizeof(scandir($root)) > 2) {
          // Not empty, back it up.
          drush_op('rename', $root, rtrim($root, '/') . '.bak');
        }
        else {
          // It is empty, just remove it.
          drush_op('drush_delete_dir', $root);
        }
      }

      $basename = basename($root);
      $options = array('--drupal-project-rename=' . $basename, 'drupal');
      $cwd = getcwd();
      drush_op('chdir', dirname($root));
      // TODO: add in profile-core-version
      $res = drush_backend_invoke_args('dl', $options);
      if ($res['error_status'] == 0) {
      }
      else {
        drush_set_error('DRUPAL_DOWNLOAD_FAILED');
        drush_delete_dir($root);
        return FALSE;
      }
      drush_op('chdir', $cwd);
      drush_log('Installed Drupal core.');
    }
  }

  if (!drush_get_option('no-symlink', FALSE) && !drush_get_option('no-core', FALSE)) {
    if (!file_exists($root . '/profile/' . $profile)) {
      symlink($build_path . '/' . $env . '-latest', $root . '/profiles/' . $profile);
      drush_log('Created profile symlink.');
    }
  }

  drush_print('Deployment set up.');
}

/**
 * Drush ding-build validate callback.
 */
function drush_ding_deploy_build_validate($path = NULL) {
  if (!drush_get_option('profile-core-version', NULL)) {
    return drush_set_error('NO_PROFILE_CORE_VERSION');
  }
  if (!drush_get_option('profile-name', NULL)) {
    return drush_set_error('NO_PROFILE_NAME');
  }
  if (!drush_get_option('profile-url', NULL)) {
    return drush_set_error('NO_PROFILE_URL');
  }
  if (!$path && !drush_get_option('root', NULL)) {
    return drush_set_error('NO_PATH_OR_ROOT');
  }
}

/**
 * Drush ding-build command callback.
 */
function drush_ding_deploy_build($path = NULL) {
  $profile = drush_get_option('profile-name', NULL);
  $profile_core_version = drush_get_option('profile-core-version', NULL);
  $profile_type = drush_get_option('profile-type', 'git');
  $profile_url = drush_get_option('profile-url', NULL);
  $profile_branch = drush_get_option('profile-branch', NULL);
  $profile_tag = drush_get_option('profile-tag', NULL);

  $makefile_content = strtr(BOOTSTRAP_MAKE, array('@core-version' => $profile_core_version, '@name' => $profile, '@type' => $profile_type, '@url' => $profile_url));
  if ($profile_branch) {
    $makefile_content .= strtr('projects[@name][download][branch] = "@branch"' . "\n", array('@name' => $profile, '@branch' => $profile_branch));
  }
  if ($profile_tag) {
    $makefile_content .= strtr('projects[@name][download][tag] = "@tag"' . "\n", array('@name' => $profile, '@tag' => $profile_tag));
  }
  $makefile = drush_save_data_to_temp_file($makefile_content);

  if (!$path) {
    $path = drush_get_option('root', NULL);
  }

  $existing = $path . '/profiles/' . $profile;
  if (file_exists($existing)) {
    if (drush_get_option('clean', NULL)) {
      if (drush_confirm(dt('Are you sure you want to delete @existing', array('@existing' => $existing)))) {
        $n = drush_delete_dir($existing);
      }
      else {
        return drush_user_abort();
      }
    }
    else {
      return drush_set_error('PATH_EXISTS');
    }
  }

  // Work around drush make not wanting to build to an existing dir, unless
  // it's '.'.
  if (file_exists($path)) {
    $old_cwd = getcwd();
    $makefile = realpath($makefile);
    chdir($path);
    $path = '.';
  }

  // Copy alias/settings/command line options to settings.
  $settings = array(
    'no-core' => TRUE,
    'contrib-destination' => '.'
  );
  if (drush_get_option('working-copy', FALSE)) {
    $settings['working-copy'] = TRUE;
  }
  $res = drush_backend_invoke_sitealias($settings, 'make', array($makefile, $path), $settings);

  if (isset($old_cwd)) {
    chdir($old_cwd);
  }

  if (!$res) {
    drush_set_error('BUILD_FAILED');
  }

  return $res;
}

/**
 * Drush ding-deploy validate callback.
 */
function drush_ding_deploy_validate() {
  if (!drush_get_option('profile-name', NULL)) {
    return drush_set_error('NO_PROFILE_NAME');
  }
  if (!drush_get_option('env', NULL)) {
    return drush_set_error('NO_ENV');
  }
  if (!drush_get_option('build-path', NULL)) {
    return drush_set_error('NO_BUILD_PATH');
  }
}

/**
 * Drush ding-deploy command callback.
 */
function drush_ding_deploy() {
  drush_set_context('DING_DEPLOY_STAGE', 'init');
  if (drush_get_option('code-only', FALSE)) {
    drush_set_option('no-offline', TRUE);
    drush_set_option('no-dump', TRUE);
    drush_set_option('no-updb', TRUE);
    drush_set_option('no-post-updb', TRUE);
  }

  $profile = drush_get_option('profile-name', NULL);
  $build_path = drush_get_option('build-path');
  $env = drush_get_option('env');
  if (!file_exists($build_path)) {
    return drush_set_error('BUILD_PATH_DOES_NOT_EXIST');
  }

  $date = date('YmdHis');
  $build_dir = $profile . '-' . $date;
  $dump_file = drush_tempnam($profile . '-' . $date . 'sql.');

  // Copy alias/settings/command line options to settings.
  $settings = drush_get_merged_options();
  $settings['build-path'] = $build_path;
  if (drush_get_option('working-copy', FALSE)) {
    $settings['working-copy'] = TRUE;
  }
  $res = drush_backend_invoke_sitealias($settings, 'ding-deploy-build', array($build_path . '/' . $build_dir), $settings);


  if ($res['error_status'] == 0) {
    drush_set_context('DING_DEPLOY_STAGE', 'offline');
    if (!drush_get_option('no-offline', FALSE)) {
      drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_DATABASE);
      $res2 = drush_backend_invoke('vset site_offline 1');
      if (!$res2 || $res2['error_status'] != 0) {
        return FALSE;
      }
    }

    drush_set_context('DING_DEPLOY_STAGE', 'dbdump');
    if (!drush_get_option('no-dump', FALSE)) {
      drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_DATABASE);
      drush_set_context('DING_DEPLOY_DBDUMP', $dump_file);
      $res2 = drush_backend_invoke('sql-dump --result-file=' . $dump_file);
      if (!$res2 || $res2['error_status'] != 0) {
        return FALSE;
      }
    }

    $cwd = getcwd();
    drush_op('chdir', $build_path);
    drush_set_context('DING_DEPLOY_STAGE', 'link-shuffle');
    if (file_exists($env . '-previous')) {
      drush_op('rename', $env . '-previous', $env . '-previous-bak');
    }
    if (file_exists($env . '-latest')) {
      drush_op('rename', $env . '-latest', $env . '-previous');
    }
    if (!symlink($build_dir . '/profiles/' . $profile, $env . '-latest')) {
      return FALSE;
    }
    drush_op('chdir', $cwd);

    drush_set_context('DING_DEPLOY_STAGE', 'updb');
    if (!drush_get_option('no-updb', FALSE)) {
      drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_FULL);
      $res2 = drush_backend_invoke('updb');
      if (!$res2 || $res2['error_status'] != 0) {
        return FALSE;
      }
    }

    drush_set_context('DING_DEPLOY_STAGE', 'post-updb');
    if (!drush_get_option('no-post-updb')) {
      drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_DATABASE);
      $post_updb = drush_get_option('post-updb', array());
      foreach ($post_updb as $command) {
        $res2 = drush_backend_invoke($command);
        if (!$res2 || $res2['error_status'] != 0) {
          return FALSE;
        }
      }
    }

    drush_set_context('DING_DEPLOY_STAGE', 'online');
    if (!drush_get_option('no-offline', FALSE)) {
      $res2 = drush_backend_invoke('vset site_offline 0');
      if (!$res2 || $res2['error_status'] != 0) {
        return FALSE;
      }
    }

    if (file_exists($dump_file)) {
      @unlink($dump_file);
    }

    $cwd = getcwd();
    drush_op('chdir', $build_path);
    @drush_op('unlink', $env . '-previous-bak');
    drush_op('chdir', $cwd);
  }
  else {
    drush_set_error('MAKE_FAILED');
    drush_delete_dir($build_dir);
    return FALSE;
  }

  drush_print('Deployment complete.');
}

/**
 * Drush ding-deploy rollback callback.
 */
function drush_ding_deploy_rollback() {
  switch (drush_get_context('DING_DEPLOY_STAGE', 'init')) {
    case 'online':
      // Odd place to fail, but should rollback everything.
    case 'post-updb':
      // Post update command failed, rollback everything.
    case 'updb':
      // Database updates failed, we must restore from backup.
      if (!drush_get_option('no-dump', FALSE)) {
        $file = drush_get_context('DING_DEPLOY_DBDUMP', '');
        if (!$file or !file_exists($file)) {
          drush_set_error('NO_DBDUMP_FILE');
        }
        else {
          $res = drush_backend_invoke('sql-connect');
          if ($res && $file) {
            $mysql = $res['output'];
            drush_op_system($mysql . ' <' . $dump_file);
          }
        }
      }
    case 'link-shuffle':
      $cwd = getcwd();
      drush_op('chdir', $build_path);

      if (file_exists($env . '-previous')) {
        drush_op('rename', $env . '-previous', $env . '-latest');
      }
      if (file_exists($env . '-previous-bak')) {
        drush_op('rename', $env . '-previous-bak', $env . '-previous');
      }
      drush_op('chdir', $cwd);
    case 'dbdump':
      // Error dumping. Not much to do other than remove the file.
      if (!drush_get_option('no-dump', FALSE)) {
        $file = drush_get_context('DING_DEPLOY_DBDUMP', '');
        if (file_exists($file)) {
          unlink($file);
        }
      }
    case 'offline':
      if (!drush_get_option('no-offline', FALSE)) {
        drush_backend_invoke('vset site_offline 0');
      }
    case 'init':
      // Nothing to roll back, we're golden.
  }
}

/**
 *
 */
function drush_ding_deploy_test() {
  drush_print('Here be dragons.');
  $test_path = '/tmp/ding-deploy-test';
  $alias_dir = dirname(__FILE__) . '/tests/aliases';

  drush_shell_exec('rm -rf ' . $test_path);
  mkdir($test_path);
  chdir($test_path);
  $bootstrap = file_get_contents('http://localhost/ding-deploy-test/bootstrap.make');
  if (!preg_match('{ding-deploy-test/basic.tar.gz}', $bootstrap)) {
    drush_log(dt("No files on localhost?"), 'error');
    return;
  }
  drush_set_option('alias-path', $alias_dir);
  $settings = drush_sitealias_get_record('@test.basic');
  $settings['alias-path'] = $alias_dir;
  $data = drush_backend_invoke_sitealias($settings, 'ding-deploy-setup', array(), $settings);

  if (empty($data['error_status'])) {
    drush_log(dt('Basic setup works.'), 'ok');
  }
  else {
    drush_log(dt("Failed basic setup."), 'error');
    return;
  }

  if (file_exists($test_path . '/build/bootstrap.make')) {
    return drush_set_error('TEST_ERROR', 'Bootstrap file exists');
  }

  $settings = drush_sitealias_get_record('@test.basic');
  $settings['alias-path'] = $alias_dir;
  $settings['code-only'] = TRUE;
  $data = drush_backend_invoke_sitealias($settings, 'ding-deploy', array(), $settings);

  if (empty($data['error_status'])) {
    drush_log(dt('Basic deploy works.'), 'ok');
  }
  else {
    drush_log(dt("Failed basic deployment."), 'error');
    return;
  }

  if (!_file_check($test_path . '/build/stg-latest')) {
    return;
  }

  if (!_file_check($test_path . '/root/profiles/basic/basic.make')) {
    return;
  }

  if (!_file_check($test_path . '/root/profiles/basic/modules/webtrends/webtrends.module')) {
    return;
  }

  $settings = drush_sitealias_get_record('@test.basic2');
  $settings['alias-path'] = $alias_dir;
  $settings['code-only'] = TRUE;
  $data = drush_backend_invoke_sitealias($settings, 'ding-deploy-build', array($test_path . '/direct'), $settings);

  if (empty($data['error_status'])) {
    drush_log(dt('Basic build works.'), 'ok');
  }
  else {
    drush_log(dt("Failed basic build."), 'error');
    return;
  }

  if (!_file_check($test_path . '/direct/profiles/basic/basic.make')) {
    return;
  }

  if (!_file_check($test_path . '/direct/profiles/basic/modules/webtrends/webtrends.module')) {
    return;
  }

  //drush_print_r($data);

}

function _file_check($file) {
  if (!file_exists($file)) {
    drush_log(dt("@file doesn't exist.", array('@file' => $file)), 'error');
    return FALSE;
  }
  return TRUE;
}