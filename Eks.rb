require 'modules/global'

RbMake.module(:Eks, :global) do |l, p|
  l.extend_with(:cpp)
  l.cpp.flags = [ :warn_as_error, :cpp14, '-stdlib=libc++' ]

  l.cpp.defines = [ :X_CPPOX_SUPPORT, l.name.to_s.upcase + "_BUILD" ]
  l.when(l.config.debug) do |l|
    l.cpp.defines << :X_DEBUG
  end

  l.when(l.config.platform(:osx)) do |l|
    l.cpp.minimum_osx_version = "10.9"
  end

  l.export do |l|
    l.include_paths << 'include'
  end
end

RbMake.module(:EksTest, :Eks) do |l, p|
  l.dependencies = [ :gtest ]
  l.sources = [ '**/*' ]
end

RbMake.import_modules('Eks*')
