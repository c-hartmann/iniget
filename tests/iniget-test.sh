
# echo base tests

echo test.ini:
cat test.ini
echo ---
echo

# # one comment
# [Foo]
# foo=bar
echo '[Foo]' 'foo' '=' 'bar' '?'
# [[ "$(../build/iniget test.ini 'Foo' 'foo')" == 'bar' ]] && echo 'okay' || echo 'fail'
[[ "$(~/.local/bin/iniget test.ini 'Foo' 'foo')" == 'bar' ]] && echo 'okay' || echo 'fail'

#
# [Bar]
# bar = BAR
# foo =FOO BAR
# foo = BAR
# foo =FOO
echo '[Bar]' 'bar' '=' 'BAR' '?'
# [[ "$(../build/iniget test.ini 'Bar' 'bar')" == 'BAR' ]] && echo 'okay' || echo 'fail'
[[ "$(~/.local/bin/iniget test.ini 'Bar' 'bar')" == 'BAR' ]] && echo 'okay' || echo 'fail'
echo '[Bar]' 'foo' '=' 'FOO' '?'
# [[ "$(../build/iniget test.ini 'Bar' 'foo')" == 'FOO' ]] && echo 'okay' || echo 'fail'
[[ "$(~/.local/bin/iniget test.ini 'Bar' 'foo')" == 'FOO' ]] && echo 'okay' || echo 'fail'

#
# [Bar Foo]
# key1=Value with spaces
# key2=Value with spaces at the end
[[ "$(../build/iniget test.ini 'Bar Foo' 'key1')" == 'Value with spaces' ]] && echo 'okay' || echo 'fail'
[[ "$(../build/iniget test.ini 'Bar Foo' 'key2')" == 'Value with spaces at the end' ]] && echo 'okay' || echo 'fail on [Bar Foo] key2'
#
# ; there must not be an empty line between sections
# [FooBar]
# bar=foo
[[ "$(../build/iniget test.ini 'FooBar' 'bar')" == 'foo' ]] && echo 'okay' || echo 'fail'
# [BarFoo]
# bar: foo
[[ "$(../build/iniget test.ini 'BarFoo' 'bar')" != 'foo' ]] && echo 'okay' || echo 'fail'
#
# [Start Space]
#   what = about spaces at the start of a line?
[[ "$(../build/iniget test.ini 'Start Space' 'what')" == 'about spaces at the start of a line?' ]] && echo 'okay' || echo 'fail'
#
# [Quoted Value]
# k="hallali hallala hallalo   "
# z=holla die "waldfee"
[[ "$(../build/iniget test.ini 'Quoted Value' 'x')" == 'hallali hallala hallalo   ' ]] && echo 'okay' || echo 'fail on k'
[[ "$(../build/iniget test.ini 'Quoted Value' 'y')" == 'holla die "waldfee"' ]] && echo 'okay' || echo 'fail on z'


echo case insensitive tests

[[ "$(../build/iniget -i test.ini 'FOO' 'FOO')" == 'bar' ]] && echo 'okay' || echo 'fail'

[[ "$(../build/iniget -i test.ini 'BAR' 'BAR')" == 'BAR' ]] && echo 'okay' || echo 'fail'
[[ "$(../build/iniget -i test.ini 'BAR' 'FOO')" == 'FOO' ]] && echo 'okay' || echo 'fail'

[[ "$(../build/iniget -i test.ini 'BAR FOO' 'KEY1')" == 'Value with spaces' ]] && echo 'okay' || echo 'fail'
[[ "$(../build/iniget -i test.ini 'BAR FOO' 'KEY2')" == 'Value with spaces at the end' ]] && echo 'okay' || echo 'fail on [Bar Foo] key2'
[[ "$(../build/iniget -i test.ini 'FOOBAR' 'BAR')" == 'foo' ]] && echo 'okay' || echo 'fail'

[[ "$(../build/iniget -i test.ini 'BARFOO' 'BAR')" != 'foo' ]] && echo 'okay' || echo 'fail'

[[ "$(../build/iniget -i test.ini 'START SPACE' 'WHAT')" == 'about spaces at the start of a line?' ]] && echo 'okay' || echo 'fail on [START SPACE] WHAT'

[[ "$(../build/iniget -i test.ini 'QUOTED VALUE' 'X')" == 'hallali hallala hallalo   ' ]] && echo 'okay' || echo 'fail on k'
[[ "$(../build/iniget -i test.ini 'QUOTED VALUE' 'Y')" == 'holla die "waldfee"' ]] && echo 'okay' || echo 'fail on z'

echo;exit

