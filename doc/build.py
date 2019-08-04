#!/usr/bin/env python3
# Build the documentation.

from __future__ import print_function
import errno, os, shutil, sys, tempfile
from subprocess import check_call, check_output, CalledProcessError, Popen, PIPE
from distutils.version import LooseVersion

versions = ['1.0.0', '1.1.0', '2.0.0', '3.0.2', '4.0.0', '4.1.0', '5.0.0', '5.1.0', '5.2.0', '5.2.1', '5.3.0']

def pip_install(package, commit=None, **kwargs):
  "Install package using pip."
  min_version = kwargs.get('min_version')
  if min_version:
    from pkg_resources import get_distribution, DistributionNotFound
    try:
      installed_version = get_distribution(os.path.basename(package)).version
      if LooseVersion(installed_version) >= min_version:
        print('{} {} already installed'.format(package, min_version))
        return
    except DistributionNotFound:
      pass
  if commit:
    package = 'git+https://github.com/{0}.git@{1}'.format(package, commit)
  print('Installing {0}'.format(package))
  check_call(['pip', 'install', package])

def create_build_env(dirname='virtualenv'):
  # Create virtualenv.
  if not os.path.exists(dirname):
    check_call(['virtualenv','--python=python3',dirname])
  import sysconfig
  scripts_dir = os.path.basename(sysconfig.get_path('scripts'))
  activate_this_file = os.path.join(dirname, scripts_dir, 'activate_this.py')
  print(scripts_dir)

  with open(activate_this_file) as f:
    exec(f.read(), dict(__file__=activate_this_file))
  import site 

  # Import get_distribution after activating virtualenv to get info about
  # the correct packages.
  from pkg_resources import get_distribution, DistributionNotFound
  # Upgrade pip because installation of sphinx with pip 1.1 available on Travis
  # is broken (see #207) and it doesn't support the show command.
  pip_version = get_distribution('pip').version
  if LooseVersion(pip_version) < LooseVersion('1.5.4'):
    print("Updating pip")
    check_call(['pip', 'install', '--upgrade', 'pip'])
  # Upgrade distribute because installation of sphinx with distribute 0.6.24
  # available on Travis is broken (see #207).
  try:
    distribute_version = get_distribution('distribute').version
    if LooseVersion(distribute_version) <= LooseVersion('0.6.24'):
      print("Updating distribute")
      check_call(['pip', 'install', '--upgrade', 'distribute'])
  except DistributionNotFound:
    pass
  # Install Sphinx and Breathe.
  pip_install('sphinx')
  pip_install('m2r')
  pip_install('recommonmark')
  pip_install('sphinx_rtd_theme')
  pip_install('michaeljones/breathe',
              '129222318f7c8f865d2631e7da7b033567e7f56a',
              min_version='4.2.0')

def build_docs(version='dev', **kwargs):
  doc_dir = kwargs.get('doc_dir', os.path.dirname(os.path.realpath(__file__)))
  work_dir = kwargs.get('work_dir', '.')
  
  include_dir = kwargs.get(
      'include_dir', os.path.join(os.path.dirname(doc_dir), 'src'))
  # Build docs.
  
  cmd = ['doxygen', '-']
  p = Popen(cmd, stdin=PIPE)
  doxygen_dir = os.path.join(work_dir, 'doxygen')
  p.communicate(input=r'''
      PROJECT_NAME      = freeUV3
      GENERATE_LATEX    = NO
      GENERATE_MAN      = NO
      GENERATE_RTF      = NO
      CASE_SENSE_NAMES  = NO
      INPUT             = {0}
      QUIET             = YES
      JAVADOC_AUTOBRIEF = YES
      AUTOLINK_SUPPORT  = NO
      GENERATE_HTML     = YES
      GENERATE_XML      = YES
      OUTPUT_DIRECTORY  = {1}
      ALIASES           = "rst=\verbatim embed:rst"
      ALIASES          += "endrst=\endverbatim"
      MACRO_EXPANSION   = YES
      PREDEFINED        = 
      EXCLUDE_SYMBOLS   = 
    '''.format(include_dir, doxygen_dir).encode('UTF-8'))
  if p.returncode != 0:
    raise CalledProcessError(p.returncode, cmd)
  html_dir = os.path.join(work_dir, 'html')
  main_versions = reversed(versions[-3:])

  check_call(['python3', 'virtualenv/bin/sphinx-build',
              '-Dbreathe_projects.freeUV3=' + os.path.abspath(doxygen_dir + '/xml' ),
              '-Dversion=' + version, '-Drelease=' + version,
              '-Aversion=' + version, '-Aversions=' + ','.join(main_versions),
              '-b', 'html', doc_dir, html_dir])
  #try:
  #  check_call(['python3','lessc', '--clean-css',
  #              '--include-path=' + os.path.join(doc_dir, 'bootstrap'),
  #              os.path.join(doc_dir, 'fmt.less'),
  #              os.path.join(html_dir, '_static', 'fmt.css')])
  #except OSError as e:
  #  if e.errno != errno.ENOENT:
  #    raise
  #  print('lessc not found; make sure that Less (http://lesscss.org/) ' +
  #        'is installed')
  #  sys.exit(1)
  return html_dir

if __name__ == '__main__':
  create_build_env()
  build_docs(sys.argv[1])
