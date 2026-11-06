#!/usr/bin/env python3
# simplewall locale builder
#
# Reads the UTF-8 translation sources from bin/i18n and writes the compiled
# language file (bin/simplewall.lng) as UTF-16LE, which is the encoding
# expected by the pinned routine library (it parses INI files through the
# GetPrivateProfile* APIs, which read UTF-16 files with a byte order mark and
# fall back to the ANSI code page for everything else).
#
# Differences against the upstream builder (henrypp/builder):
#   - UTF-8 input for the translation sources, UTF-16LE output for the
#     compiled language file
#   - keeps the English section names, which are required by the pinned
#     routine library to match the language of the system
#   - does not renumber the resource identifiers
#   - does not rewrite the translation sources

import argparse
import os
import shutil
import sys


def log_status (title):
	print ('[locale] ' + title)


def parse_resource_header (path):
	# returns { 'IDS_FOO': 1, ... }
	strings = {}

	with open (path, 'r', encoding='utf-8-sig') as fn:
		for line in fn:
			line = line.strip ()

			if not line.startswith ('#define '):
				continue

			parts = line.split ()

			if len (parts) < 3:
				continue

			name = parts[1]
			value = parts[2]

			if name.startswith ('IDS_') and value.isdigit ():
				strings[name] = int (value)

	return strings


def parse_resource_code (path):
	# returns { 'IDS_FOO': 'default text', ... }
	defaults = {}

	with open (path, 'r', encoding='utf-8-sig') as fn:
		for line in fn:
			line = line.strip (' \t\r\n')

			if not line.startswith ('IDS_'):
				continue

			parts = line.partition (' ')

			if not parts[0] or not parts[2]:
				continue

			key = parts[0].strip (' \t"')

			start_pos = parts[2].find ('"')

			if start_pos < 0:
				continue

			end_pos = parts[2].rfind ('"')

			if end_pos <= start_pos:
				continue

			value = parts[2][start_pos + 1:end_pos].replace ('""', '"').strip (' \t"')

			if key and value:
				defaults[key] = value

	return defaults


def parse_locale (path):
	# returns (description, { key: value })
	description = []
	values = {}
	is_description = True

	with open (path, 'r', encoding='utf-8-sig') as fn:
		for line in fn:
			stripped = line.strip (' \t\r\n')

			if is_description:
				if stripped.startswith (';'):
					description.append (line.rstrip ('\r\n'))
					continue

				is_description = False

			if not stripped or stripped.startswith (';'):
				continue

			parts = stripped.partition ('=')

			if not parts[0] or not parts[2]:
				continue

			key = parts[0].strip (' \t')
			value = parts[2].strip (' \t')

			values[key] = value

	return description, values


def main ():
	parser = argparse.ArgumentParser (add_help=False, description='Build project locale.')
	parser.add_argument ('--name-short', help='project short name', required=True)

	args = parser.parse_args ()

	app_name_short = args.name_short

	project_directory = os.path.dirname (os.path.abspath (__file__))
	i18n_directory = os.path.join (project_directory, 'bin', 'i18n')
	locale_file = os.path.join (project_directory, 'bin', app_name_short + '.lng')
	resource_rc = os.path.join (project_directory, 'src', 'resource.rc')
	resource_h = os.path.join (project_directory, 'src', 'resource.h')

	log_status ('Checking configuration')

	for name, path in (('Resource header', resource_h), ('Resource code', resource_rc)):
		if not os.path.isfile (path):
			log_status ('Failed: "' + name + '" was not found.')
			return 1

	if not os.path.isdir (i18n_directory):
		log_status ('Failed: locale directory was not found.')
		return 1

	log_status ('Parse resource header')

	strings = parse_resource_header (resource_h)

	if not strings:
		log_status ('Failed: no string resources were found.')
		return 1

	log_status ('Parse resource code')

	defaults = parse_resource_code (resource_rc)

	if not defaults:
		log_status ('Failed: no string defaults were found.')
		return 1

	# enumerate localizations
	i18n_files = sorted (name for name in os.listdir (i18n_directory) if name.endswith ('.ini'))

	if not i18n_files:
		log_status ('Failed: no translation sources were found.')
		return 1

	# get the localization timestamp
	locale_timestamp = 0

	for name in i18n_files:
		lastmod = os.path.getmtime (os.path.join (i18n_directory, name))

		if lastmod > locale_timestamp:
			locale_timestamp = lastmod

	locale_timestamp = int (locale_timestamp)

	# write the language content
	log_status ('Write updated language content')

	lng_content = '; ' + app_name_short + '\n'
	lng_content = lng_content + '; https://github.com/henrypp/' + app_name_short + '/tree/master/bin/i18n\n'
	lng_content = lng_content + ';\n; DO NOT MODIFY THIS FILE -- YOUR CHANGES WILL BE ERASED!\n\n'

	for index, name in enumerate (i18n_files):
		locale_name = os.path.splitext (name)[0]
		locale_path = os.path.join (i18n_directory, name)

		description, values = parse_locale (locale_path)

		description = ('\n'.join (description) + '\n\n') if description else ''

		lng_content = lng_content + description + '[' + locale_name + ']\n'

		# write the locale timestamp
		if locale_name.lower () == 'russian':
			lng_content = lng_content + '000=' + str (locale_timestamp) + '\n'

		for key in strings:
			default_value = defaults.get (key)

			if not default_value:
				continue

			number_key = '{:03}'.format (strings[key])

			if number_key in values:
				value = values[number_key]

			elif key in values:
				value = values[key]

			else:
				value = default_value

			# write only localized strings
			if value != default_value:
				lng_content = lng_content + number_key + '=' + value + '\n'

		# line-ending hack
		if index < len (i18n_files) - 1:
			lng_content = lng_content + '\n'

	if os.path.isfile (locale_file):
		os.chmod (locale_file, 0o666)

	with open (locale_file, 'w', encoding='utf-16', newline='\r\n') as fn:
		fn.write (lng_content)

	log_status ('Write locale "' + os.path.basename (locale_file) + '" was finished')

	# copy the localization to the binaries folders
	for folder in ('64', 'ARM64'):
		directory = os.path.join (project_directory, 'bin', folder)

		if os.path.isdir (directory):
			shutil.copyfile (locale_file, os.path.join (directory, os.path.basename (locale_file)))

	log_status ('Locale timestamp: ' + str (locale_timestamp) + '.')

	return 0


if __name__ == '__main__':
	sys.exit (main ())
