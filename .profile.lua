-- Macro support
macros = {
  ['^%$%s*%S+'] = function(line)
    net.send('EXEC', string.format("return os.shell('%s')", line:sub(2)))
    return true
  end
}

function client_prompt(line)
  for pattern, macro in pairs(macros) do
    if line:match(pattern) then
      return macro(line)
    end
  end
end

-- Start in home
os.path(HOME)

print('Profile loaded')