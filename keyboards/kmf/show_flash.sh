arm-none-eabi-nm -Crtd --size-sort $1 | grep -i ' [t] ' | sed -e 's#^0000000#       #g' -e 's#^000000#      #g' -e 's#^00000#     #g' -e 's#^0000#    #g' -e 's#^000#   #g' -e 's#^00#  #g' -e 's#^0# #g'
