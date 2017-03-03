-- Author: Louis Moureaux
-- Year:   2017

local math = require('math')
local os = require('os')

function printusage()
  print('Usage: ' .. arg[0] .. ' <datafile> <action> <args>...')
  print('Where:')
  print('  <datafile> is a file containing the output of hbanalyzer')
  print('  <action> is one of the following: slice, energy, hotcells')
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
  if #arg >= 3 then
    local numhot = tonumber(arg[3])
    print('ieta', 'eta', 'energy')
    for ieta, tab in pairs(data[numhot]) do
      print(ieta, (ieta + 0.5) * 0.085, tab.energy)
    end
  else
    print('Error: missing parameter for action \'slice\'')
    os.exit(1)
  end
elseif arg[2] == 'energy' then
  if #arg >= 3 then
    ok, spec = pcall(loadfile(arg[3]))
    if not ok then
      print('Error: cannot load \'' .. arg[3] .. '\'')
      os.exit(1)
    end
    print('ieta', 'eta', 'numhot', 'energy')
    print(-17, -1.4025, 0, 0)
    for ieta = -16, 15 do
      numhot = spec[ieta]
      print(ieta, (ieta + 0.5) * 0.085, numhot, data[numhot][ieta].energy)
    end
    print(16, 1.4025, 0, 0)
  else
    print('Error: missing parameter for action \'energy\'')
    os.exit(1)
  end
elseif arg[2] == 'hotcells' then
  if #arg >= 3 then
    ok, spec = pcall(loadfile(arg[3]))
    if not ok then
      print('Error: cannot load \'' .. arg[3] .. '\'')
      os.exit(1)
    end
    print('ieta', 'eta', 'iphi', 'phi')
    for ieta, numhot in pairs(spec) do
      for _, iphi in ipairs(data[numhot][ieta]) do
        print(ieta, (ieta + 0.5) * 0.085, iphi, (iphi + 0.5) * math.pi / 36)
      end
    end
  else
    print('Error: missing parameter for action \'hotcells\'')
    os.exit(1)
  end
else
  print('Error: unknown action: \'' .. arg[2] .. '\'')
  os.exit(1)
end
