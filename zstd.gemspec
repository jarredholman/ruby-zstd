require File.expand_path('../lib/zstd/version', __FILE__)

Gem::Specification.new do |s|
  s.name = 'zstd'
  s.version = Zstd::VERSION
  s.summary = ''
  s.author = 'Jarred Holman'
  s.licenses = ['MIT']

  s.files = open('|git ls-files','r').readlines.map(&:chomp)
  
  s.platform = Gem::Platform::RUBY
  s.extensions << 'ext/zstd/extconf.rb'

  s.require_paths = ['lib', 'ext']

  s.add_development_dependency 'rspec'
  s.add_development_dependency 'rake-compiler', '~> 1.0'
end
