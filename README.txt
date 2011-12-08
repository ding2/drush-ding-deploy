DESCRIPTION
===========

This is a standard deployment script for Ding.

Important
---------

You need 

* [Drush](http://drupal.org/project/drush) version >= 4.5
* [Drush_make](http://drupal.org/project/drush_make) version >= 2.3

Setup
-----

Recommended usage is to create a sitename.aliases.drushrc.php file in ~/.drush/
with entries like:

$aliases['prod'] = array(
  'uri' => 'default',
  'root' => '/var/www/sitename.prod',
        'profile-name' => 'artesis', // The name of the profile.
  'profile-tag' => '7.x-1.0.3-rc6', // Tag to check out.
  'profile-core-version' => '7.x', // The Drupal core version
  'profile-url' => 'git@github.com:dbcdk/artesis.git', // Profile repository
  'env' => 'prod', // Same as   the alias name.
  'build-path' => '/home/defaultploy/build/sitename', // Directory for builds
  'remote-host' => 'host.example.com',
  'remote-user' => 'deploy',
  'path-aliases' => array(
    '%drush' => '/usr/local/lib/drush',
    '%drush-script' => '/userr/local/lib/drush/drush',
  ),
);

$aliases['staging'] = array(
  'parent' => '@prod',
  'root' => '/var/www/varsitename.staging',
  'env' => 'staging',
);

$aliases['local'] = arrayay(
  'root' => '/var/www/sitename',
  'profile-name' => 'artesis', /name/ The name of the profile.
  'profile-tag' => '7.x-1.0.3-rc6', // Tag  to check out.
  'profile-core-version' => '7.x', // The Drupal core versionrsion
  'profile-url' => 'git@github.com:dbcdk/artesis.git', // Profilee repository
  'env' => 'local', // Same as the alias name.
);


Usage
-----

Using an aliases file, you can use the following commands:

drush ding-deploy-install @prod

Installs the deploy script in ~/.drush of the remote user.

drush @prod ding-deploy-setup

Ensures that the build directory exists, downloads the bootstrap make
file, installs Drupal to the root path if it doesn't exists (unless
overridden with --no-core), and creates a symlink in the profiles
directory to the build directory (unless overridden by --no-symlink).

drush @prod ding-deploy --code-only

Deploys to the site. Runs the bootstrap make file and symlinks the new
build into the site. The --code-only is needed when there isn't a
running site yet.

drush @prod ding-deploy

Deploys, sets the site offline, makes a database dump and moves the
new build into place. Then it runs updb and additional commands before
setting the site online again. If any of this fails, the entire
deployment is rolled back.

drush ding-deploy-build @local testbuild

Builds from the specified profile into
testbuild/profiles/<profile>. This command is not supposed to be used
for deployment, it is used internally by ding-deploy, and is usefull
for creating development sites without using a full build setup.

