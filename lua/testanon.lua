readdicom('0009703828:1.3.46.670589.5.2.10.2156913941.892665340.475317')
Data.Dump('image_in.txt')
dofile('lua/anonymize_script.lua')
Data.Dump('image_anonymized.txt')
dofile('lua/deanonymize_script.lua')
Data.Dump('image_restored.txt')