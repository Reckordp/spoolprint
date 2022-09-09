require 'rake/testtask'

Rake::TestTask.new do |t|
  t.libs << 'test'
end

task: "halo" do
  sh "echo a > in.dist"
end

desc "Run tests"
task default: :test