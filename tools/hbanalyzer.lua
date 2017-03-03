-- Author: Louis Moureaux
-- Year:   2017

local os = require('os')

function printusage()
  print('Usage: ' .. arg[0] .. ' <datafile> <action> <args>...')
  print('Where:')
  print('  <datafile> is a file containing the output of hbanalyzer')
  print('  <action> is one of the following: slice')
  print('  <args> depend on the specified action')
end

-- Arguments
if #arg < 2 then
  print('Error: not enough arguments')
  printusage()
  os.exit(1)
end

-- Load file
filename = arg[1]
ok, data = pcall(loadfile(filename))
if not ok then
  print('Error: cannot load \'' .. filename .. '\'')
  os.exit(1)
end

-- Execute action
if arg[2] == 'slice' then
  if arg[3] then
    local numhot = tonumber(arg[3])
    print('ieta', 'eta', 'energy')
    for ieta, tab in pairs(data[numhot]) do
      print(ieta, (ieta + 0.5) * 0.085, tab.energy)
    end
  else
    print('Error: missing parameter for action \'nslice\'')
    os.exit(1)
  end
else
  print('Error: unknown action: \'' .. arg[2] .. '\'')
  os.exit(1)
end
