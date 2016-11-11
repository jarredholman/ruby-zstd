require "rake/extensiontask"

if Gem.win_platform?
  task :devkit do
    begin
      require 'devkit'
    rescue LoadError
      warn 'Failed to load devkit, installation might fail'
    end
  end

  task :compile => [:devkit]
end

GEMSPEC = Gem::Specification.load('zstd.gemspec')

Rake::ExtensionTask.new("zstd", GEMSPEC) do |task|
  task.lib_dir = "lib/zstd"
  task.ext_dir = "ext/zstd"
end
