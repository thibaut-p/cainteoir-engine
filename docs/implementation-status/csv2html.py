#!/usr/bin/python

import os
import sys

class StringFormat:
	def __init__(self, visible=True):
		self.visible = visible
		self.implementation = False

	def parse(self, values):
		return values[0]

class CommentFormat:
	def __init__(self, visible=True):
		self.visible = visible
		self.implementation = False

	def parse(self, values):
		value = ','.join(values)
		return value.replace('<', '&lt;').replace('>', '&gt;')

class UrlFormat:
	def __init__(self, visible=True):
		self.visible = visible
		self.implementation = False

	def parse(self, values):
		return values[0].replace('.csv', '.html')

class StatusFormat:
	def __init__(self, visible=True, implementation=False):
		self.visible = visible
		self.implementation = implementation

	def parse(self, values):
		if values[0] == 'no':
			return ('failure', 'No')
		if values[0] == 'na':
			return ('na', 'N/A')
		if values[0].startswith('*'):
			return ('inprogress', values[0].replace('*', ''))
		return ('success', values[0])

class StatusSetFormat:
	def __init__(self, visible=True):
		self.visible = visible
		self.implementation = False

	def parse(self, values):
		status = StatusFormat()
		ret = []
		for value in values:
			x, c = value.split('/')
			if c in ['no', 'na']:
				b = x
				a, _ = status.parse(c)
			else:
				a, b = status.parse(x)
			ret.append((a, b, c))
		return ret

properties = {
	'comments': CommentFormat(),
	'implemented': StatusFormat(implementation=True),
	'model': StatusFormat(implementation=True),
	'parsing': StatusFormat(implementation=True),
	'rdf': StatusFormat(implementation=True),
	'section': StringFormat(),
	'tests': StatusFormat(),
	'title': StringFormat(),
	'toc': StatusFormat(implementation=True),
	'tts': StatusFormat(implementation=True),
	'url': UrlFormat(visible=False),
	'version': StringFormat(),
	'versions': StatusSetFormat(),
}

types = {
	'css-spec': ['section', 'title', 'url', 'model', 'parsing', 'tests', 'comments'],
	'format': ['title', 'version', 'url', 'implemented', 'tts', 'rdf', 'toc', 'comments'],
	'formats': [ 'title', 'url', 'versions' ],
	'spec': ['section', 'title', 'url', 'implemented', 'tests', 'comments'],
	'standard': ['version', 'url', 'implemented', 'comments'],
}

categories = {
	'document': 'Document Format',
	'metadata': 'Metadata Export',
}

def parse_csv(filename):
	ref  = filename.replace('.csv', '')
	data = { "support": [], 'category': 'document', 'items': 0, 'success': 0, 'failure': 0, 'inprogress': 0, 'na': 0 }
	with open(filename) as f:
		for line in f:
			line = line.replace('\n', '')
			if line == '' or line.startswith('#'): # comment or blank line
				pass
			elif line.startswith('@'):
				try:
					key, value = line.replace('@', '').split(',')
					if key == 'references':
						if value == '':
							data[key] = []
						else:
							data[key] = value.split(' ')
					else:
						data[key] = value
				except ValueError:
					raise Exception('line "%s" contains too many \',\'s' % line)
			else:
				try:
					s = line.split(',')
					props = {}
					for prop in types[data['type']]:
						p = properties[prop]
						props[prop] = p.parse(s)
						if p.implementation:
							istatus, ivalue = props[prop]
							data[istatus] = data[istatus] + 1
							data['items'] = data['items'] + 1
						s = s[1:]
					data['support'].append(props)
				except ValueError:
					raise Exception('line "%s" contains too many \',\'s' % line)
	return ref, data

def print_url(data, ref, classname=None):
	if data['url'] == '':
		if classname:
			f.write('\t\t<td class="%s">%s</td>\n' % (classname, data[ref].replace(' ', '&#xA0;')))
		else:
			f.write('\t\t<td>%s</td>\n' % data[ref].replace(' ', '&#xA0;'))
	else:
		if classname:
			f.write('\t\t<td class="%s"><a href="%s">%s</a></td>\n' % (classname, data['url'], data[ref].replace(' ', '&#xA0;')))
		else:
			f.write('\t\t<td><a href="%s">%s</a></td>\n' % (data['url'], data[ref].replace(' ', '&#xA0;')))

def print_status(data, ref=None):
	status, label = data[ref]
	f.write('\t\t<td class="%s">%s</td>\n' % (status, label))

specs = {}
for csv in os.listdir('.'):
	if csv.endswith('.csv'):
		ref, data = parse_csv(csv)
		specs[ref] = data

for ref, spec in specs.items():
	print 'generating %s.html ...' % ref
	with open('%s.html' % ref, 'w') as f:
		f.write('---\n')
		f.write('layout: rdfa\n')
		if spec['type'] == 'spec':
			title = '%s %s %s Support' % (spec['name'], spec['version'], categories[spec['category']])
			f.write('title: %s\n' % title)
			f.write('description: The state of %s %s specification implementation in the Cainteoir Text-to-Speech program.\n' % (spec['name'], spec['version']))
		elif spec['type'] == 'css-spec':
			title = '%s %s Support' % (spec['name'], spec['version'])
			f.write('title: %s\n' % title)
			f.write('description: The state of %s %s specification implementation in the Cainteoir Text-to-Speech program.\n' % (spec['name'], spec['version']))
		else:
			if spec['name'] == categories[spec['category']]:
				title = '%s Support' % spec['name']
			else:
				title = '%s %s Support' % (spec['name'], categories[spec['category']])
			f.write('title: %s\n' % title)
			f.write('description: The state of %s implementation in the Cainteoir Text-to-Speech program.\n' % spec['name'])
		f.write('keywords: text to speech, tts, cainteoir, %s\n' % spec['name'].lower())
		f.write('nav:\n')
		f.write('  - { title: Home , url: ../index.html }\n')
		if ref in categories.keys():
			f.write('  - { title: %s Support }\n' % categories[ref])
		else:
			f.write('  - { title: %s Support , url: %s.html }\n' % (categories[spec['category']], spec['category']))
			if spec['version'] == '':
				f.write('  - { title: %s }\n' % spec['name'])
			elif 'parent' in spec.keys():
				f.write('  - { title: %s , url: %s.html }\n' % (specs[spec['parent']]['name'], spec['parent']))
				f.write('  - { title: %s }\n' % spec['version'])
			else:
				f.write('  - { title: %s , url: %s.html }\n' % (spec['name'], spec['name'].lower().replace('/', '')))
				f.write('  - { title: %s }\n' % spec['version'])
		if 'references' in spec.keys() and len(spec['references']) != 0:
			f.write('sidebar:\n')
			f.write('  - { title: Implementation Status , url: "#status" }\n')
			f.write('  - { title: References , url: "#references" }\n')
		f.write('---\n')
		f.write('<h1>%s</h1>\n' % title)
		if spec['type'] != 'formats':
			f.write('<h2 id="status">Implementation Status</h2>\n')
			base = spec['items'] - spec['na']
			completed = 0
			if base != 0:
				completed = int((float(spec['success']) / base) * 100)
			if completed == 0:
				f.write('<div style="border: 1px solid #ddd;" class="na">%d%%</div>' % completed)
			else:
				f.write('<div style="border: 1px solid #ddd; padding: 0;" class="na">')
				f.write('<div style="border: 1px solid #080; width: %d%%;" class="success">%d%%</div>' % (completed, completed))
				f.write('</div>')
		f.write('<table style="width: 100%;">\n')
		if spec['type'] == 'formats':
			for i in range(0, 9):
				f.write('\t\t<col width="10%"/>\n')
		else:
			f.write('\t<tr>\n')
			if spec['type'] == 'spec':
				f.write('\t\t<th style="width: 5%;">Section</th>\n')
				f.write('\t\t<th style="width: 25%;">Title</th>\n')
				f.write('\t\t<th style="width: 10%;">Implemented</th>\n')
				f.write('\t\t<th style="width: 10%;">Tests</th>\n')
				f.write('\t\t<th style="width: 50%;">Comments</th>\n')
			elif spec['type'] == 'css-spec':
				f.write('\t\t<th style="width: 5%;">Section</th>\n')
				f.write('\t\t<th style="width: 25%;">Title</th>\n')
				f.write('\t\t<th style="width: 10%;">Model</th>\n')
				f.write('\t\t<th style="width: 10%;">Parsing</th>\n')
				f.write('\t\t<th style="width: 10%;">Tests</th>\n')
				f.write('\t\t<th style="width: 40%;">Comments</th>\n')
			elif spec['type'] == 'format':
				f.write('\t\t<th style="width: 10%;">Name</th>\n')
				f.write('\t\t<th style="width: 10%;">Version</th>\n')
				f.write('\t\t<th style="width: 10%;">Text</th>\n')
				f.write('\t\t<th style="width: 10%;">Metadata</th>\n')
				f.write('\t\t<th style="width: 10%;">Table of Content</th>\n')
				f.write('\t\t<th style="width: 50%;">Comments</th>\n')
			else:
				f.write('\t\t<th style="width: 5%;">Version</th>\n')
				f.write('\t\t<th style="width: 10%;">Implemented</th>\n')
				f.write('\t\t<th style="width: 85%;">Comments</th>\n')
			f.write('\t</tr>\n')
		for data in spec['support']:
			f.write('\t<tr>\n')
			if spec['type'] == 'spec':
				f.write('\t\t<td>%s</td>\n' % data['section'])
				print_url(data, 'title')
				print_status(data, 'implemented')
				print_status(data, 'tests')
				f.write('\t\t<td>%s</td>\n' % data['comments'])
			elif spec['type'] == 'css-spec':
				f.write('\t\t<td>%s</td>\n' % data['section'])
				print_url(data, 'title')
				print_status(data, 'model')
				print_status(data, 'parsing')
				print_status(data, 'tests')
				f.write('\t\t<td>%s</td>\n' % data['comments'])
			elif spec['type'] == 'format':
				print_url(data, 'title', classname=data['implemented'][0])
				f.write('\t\t<td>%s</td>\n' % data['version'])
				print_status(data, 'tts')
				print_status(data, 'rdf')
				print_status(data, 'toc')
				f.write('\t\t<td>%s</td>\n' % data['comments'])
			elif spec['type'] == 'formats':
				print_url(data, 'title')
				for status, label, impl in data['versions']:
					f.write('\t\t<td class="%s" style="width: 10%%;" title="Supported since %s">%s</td>\n' % (status, impl, label))
			else:
				print_url(data, 'version')
				print_status(data, 'implemented')
				f.write('\t\t<td>%s</td>\n' % data['comments'])
			f.write('\t</tr>\n')
		f.write('</table>\n')
		if 'references' in spec.keys() and len(spec['references']) != 0:
			f.write('<h2 id="references">References</h2>\n')
			f.write('<ol class="references">\n')
			for ref in spec['references']:
				with open('%s.xml' % ref) as r:
					f.write(r.read())
			f.write('</ol>\n')
