from setuptools import setup

setup(name='workflow_executor',
      version='1.0.0',
      description='Script to run cwl workflow on kubernetes environment',
      url='',
      author='Blasco Brauzzi',
      author_email='blasco.brauzzi@terradue.com',
      license='MIT',
      packages=['workflow_executor','rm_client'],
      package_data={'workflow_executor':['*']},
      entry_points={
          'console_scripts': ['workflow_executor=workflow_executor.client:main'],
      },
      zip_safe=False)
