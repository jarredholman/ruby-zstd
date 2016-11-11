require File.expand_path('../lib/zstd/version', __FILE__)

Gem::Specification.new do |s|
  s.name = 'zstd'
  s.version = Zstd::VERSION
  s.summary = 'Wrapper around zstd (without external dependency)'
  s.author = 'Jarred Holman'
  s.licenses = ['MIT']
  s.homepage = 'https://github.com/jarredholman/ruby-zstd'

  s.files = open('|git ls-files','r').readlines.map(&:chomp)
  
  s.platform = Gem::Platform::RUBY
  s.extensions << 'ext/zstd/extconf.rb'

  s.require_paths = ['lib', 'ext']

  s.add_development_dependency 'rspec', '~> 0'
  s.add_development_dependency 'rake-compiler', '~> 1.0'
end
