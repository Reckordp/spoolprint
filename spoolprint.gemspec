require File.expand_path('../lib/spoolprint/version', __FILE__)

Gem::Specification.new do |s|
  s.name = "spoolprint"
  s.version = Spoolprint::VERSION
  s.license = "MIT"
  s.authors = ["Reckordp"]
  s.summary = "Library for printing to Esc/Eps printer"
  s.description = "Ruby/Spoolprint provides way to connecting printer on Windows"
  s.email = "paksaambil@gmail.com"
  s.homepage = "https://github.com/Reckordp/spoolprint"
  s.add_dependency(%q(escpos), [">= 0"])
  s.require_paths = ["lib"]
  s.date = "2022-09-08"
  s.files = `git ls-files`.split($\)
  s.extensions = "ext/native/extconf.rb"
end