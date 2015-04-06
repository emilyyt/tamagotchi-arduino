def format_hex(hex)
  hex_s = hex.to_s
  new_hex = ""
  for i in (0..hex_s.length)
    if i == hex_s.length
      break
    end
    if i%2 == 0 
       new_hex = new_hex + "0x" + hex_s[i]
    else
      new_hex = new_hex + hex_s[i] + ", "
    end

  end
  new_hex
end

puts format_hex("788402012521212501028478322a27a3e12121e1a3272a32")

