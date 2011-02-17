<?php

/**
 * @file
 *   Ding deployment functions.
 */

define('DING_DEPLOY_GIT_REPO', 'https://github.com/dingproject/drush-ding-deploy.git');
define('DEFAULT_BOOTSTRAP_FILE', 'http://localhost/test2/test.make');

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
      'profile-name' => 'The name of the profile, defaults to ding2',
      'build-path' => 'Full path to build directory',
      'boostrap' => 'Bootstrap drush_make file. Fetched and put in the build directory',
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
    'description' => "Set up server for future deployments of site.",
    'options' => array(
      'profile-name' => 'The name of the profile, defaults to ding2',
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
      'make-file' => 'Bootstrap make file to use. If not specified, use the bootstrap.make file in the build-path',
      'build-path' => 'Full path to build directory, not used if --make-file is specified',
      'working-copy' => 'Create checkouts when possible',
    ),
    'examples' => array(
      'drush @dev ding-build /var/www/drupal-7.x/profiles/ding2' => 'Build ding in the specified dir.',
    ),
    'aliases' => array('ddbuild'),
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
    case 'error:NO_REMOTE_USER':
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
    case 'error:ERROR_DOWNLOADING_BOOTSTRAP':
      return dt("Could not download bootstrap file.");
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
    case 'error:NO_BUILD_PATH_OR_MAKE_FILE':
      return dt("Either build path or make-file needs to be specified.");
    case 'error:NO_PATH_OR_ROOT':
      return dt("Root dir needs to be specified if omitting path.");
  }
}

/**
 * Drush ding-deploy-install validate callback.
 */
function drush_ding_deploy_install_validate($alias) {
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
  $bootstrap = drush_get_option('bootstrap', DEFAULT_BOOTSTRAP_FILE);
  $profile = drush_get_option('profile-name', 'ding2');
  $build_path = drush_get_option('build-path');
  $env = drush_get_option('env');
  if (!file_exists($build_path)) {
    if (!drush_mkdir($build_path)) {
      return drush_set_error('ERROR_CREATING_BUILD_PATH');
    }
    drush_log('Created build dir.');
  }

  if (file_exists($build_path . '/bootstrap.make')) {
    drush_log('Makefile exists, not overwriting');
  }
  else {
    if (!drush_shell_exec("wget --no-check-certificate -O %s %s", $build_path . '/bootstrap.make', $bootstrap)) {
      // Wget creates the file even if failing.
      unlink($build_path . '/bootstrap.make');
      return drush_set_error('ERROR_DOWNLOADING_BOOTSTRAP');
    }
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

  if (!drush_get_option('no-symlink', FALSE)) {
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
  if (!drush_get_option('make-file', NULL) && !drush_get_option('build-path', NULL)) {
    return drush_set_error('NO_BUILD_PATH_OR_MAKE_FILE');
  }
  if (!$path && !drush_get_option('root', NULL)) {
    return drush_set_error('NO_PATH_OR_ROOT');
  }
}

/**
 * Drush ding-build command callback.
 */
function drush_ding_deploy_build($path = NULL) {
  $makefile = drush_get_option('make-file', NULL);
  if (!$makefile) {
    $makefile = drush_get_option('build-path', NULL) . '/bootstrap.make';
  }

  if (!$path) {
    $path = drush_get_option('root', NULL);
  }

  if (drush_get_option('clean', NULL)) {
    $profile = drush_get_option('profile-name', 'ding2');
    $existing = $path . '/profiles/' . $profile;
    if (file_exists($existing)) {
      if (drush_confirm(dt('Are you sure you want to delete @existing', array('@existing' => $existing)))) {
        $n = drush_delete_dir($existing);
      }
      else {
        return drush_user_abort();
      }
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

  $options = array('--no-core', '--contrib-destination=.', $makefile, $path);
  if (drush_get_option('working-copy', FALSE)) {
    $options[] = '--working-copy';
  }

  $res = drush_backend_invoke_args('make', $options);

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

  $profile = drush_get_option('profile-name', 'ding2');
  $build_path = drush_get_option('build-path');
  $env = drush_get_option('env');
  if (!file_exists($build_path)) {
    return drush_set_error('BUILD_PATH_DOES_NOT_EXIST');
  }

  $date = date('YmdHis');
  $build_dir = $profile . '-' . $date;
  $dump_file = drush_tempnam($profile . '-' . $date . 'sql.');

  $options = array('--build-path=' . $build_path, $build_path . '/' . $build_dir);
  if (drush_get_option('working-copy', FALSE)) {
    $options[] = '--working-copy';
  }
  $res = drush_backend_invoke_args('ding-deploy-build', $options);


  if ($res['error_status'] == 0) {
    drush_set_context('DING_DEPLOY_STAGE', 'offline');
    if (!drush_get_option('no-offline', FALSE)) {
      drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_DATABASE);
      $res2 = drush_backend_invoke('vset site_offline 1');
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
    symlink($build_dir, $env . '-latest');
    drush_op('chdir', $cwd);

    drush_set_context('DING_DEPLOY_STAGE', 'dbdump');
    if (!drush_get_option('no-dump', FALSE)) {
      drush_bootstrap(DRUSH_BOOTSTRAP_DRUPAL_DATABASE);
      drush_set_context('DING_DEPLOY_DBDUMP', $dump_file);
      $res2 = drush_backend_invoke('sql-dump --result-file=' . $dump_file);
      if (!$res2 || $res2['error_status'] != 0) {
        return FALSE;
      }
    }

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
    case 'dbdump':
      // Error dumping. Not much to do other than remove the file.
      if (!drush_get_option('no-dump', FALSE)) {
        $file = drush_get_context('DING_DEPLOY_DBDUMP', '');
        if (file_exists($file)) {
          unlink($file);
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
    case 'offline':
      if (!drush_get_option('no-offline', FALSE)) {
        drush_backend_invoke('vset site_offline 0');
      }
    case 'init':
      // Nothing to roll back, we're golden.
  }
}