

DESCRIPTION
===========

This is a standard deployment script for Ding.

Important
---------

For this to work, you have to patch drush_make module (http://drupal.org/node/1016924). 

The patch to implement is http://drupal.org/files/issues/recursive-versions.patch

patch -p1 < recursive-versions.patch


Setup
-----

Recommended usage is to create a sitename.aliases.drushrc.php file
with entries like:

$aliases['prod'] = array(
  'uri' => 'default',
  'root' => '/var/www/sitename.prod',
  'profile-name' => 'ding', // The name of the profile.
  'env' => 'prod', // Same as the alias name.
  'build-path' => '/home/deploy/build/sitename', // Directory for builds
  'remote-host' => 'host.example.com',
  'remote-user' => 'deploy',
  'post-updb' => array(
    'status', // Drush commands to run post updb.
  ),
  'path-aliases' => array(
    '%drush' => '/usr/local/lib/drush',
    '%drush-script' => '/usr/local/lib/drush/drush',
  ),
);

$aliases['staging'] = array(
  'parent' => '@prod',
  'root' => '/var/www/sitename.staging',
  'env' => 'staging',
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

drush @prod ding-deploy

Deploys, sets the site offline, makes a database dump and moves the
new build into place. Then it runs updb and additional commands before
setting the site online again. If any of this fails, the entire
deployment is rolled back.

drush ding-deploy-build --make-file=bootstrap.make testbuild

Builds from the specified bootstrap file into
testbuild/profiles/<profile>. This command is not supposed to be used
for deployment, it is used internally by ding-deploy, and is usefull
for creating development sites without using a full build setup.
