
require('luaunit')
require('utils')


TestMovements = {} --class
    function TestMovements:setUp() 
        clearBuffer()
        sendkeys( "<ESC>" )
    end

    function TestMovements:tearDown() 
        clearBuffer()
    end

    function TestMovements:test_initial_state()
        assertEquals(bufferContent(),"")
        assertPos(1,1)
        assertEquals(mode(), MODE_NORMAL)

        sendkeys( "i" )
        --printBufferContent()
        assertEquals(mode(), MODE_INSERT)
        assertPos(1,1)
        assertEquals(bufferContent(),"")
    end

    function TestMovements:test_insert_mode()
        assertEquals( bufferContent(), "" )
        sendkeys( "i" )
        assertEquals( bufferContent(), "" )
        assertEquals(mode(), MODE_INSERT)
        assertPos( 1, 1 )

        sendkeys( "i23" )
        assertEquals( bufferContent(), "i23" )
        assertEquals(mode(), MODE_INSERT)
        assertPos( 1, 4 )

        sendkeys( "<ENTER>456" )
        assertEquals( bufferContent(), "i23\n456" )
        assertEquals(mode(), MODE_INSERT)
        assertPos( 2, 4 )

        sendkeys( "<ESC>" )
        assertEquals( bufferContent(), "i23\n456" )
        assertEquals(mode(), MODE_NORMAL)
        assertPos( 2, 3 )

        sendkeys( "<ESC>" )
        assertEquals( bufferContent(), "i23\n456" )
        assertEquals(mode(), MODE_NORMAL)
        assertPos( 2, 3 )
    end

    function TestMovements:test_char_movement()
        sendkeys( "<ESC>" )
		clearBuffer()
        sendkeys( "i0123<ENTER>4567<ENTER>89AB<ENTER>CDEF<ESC>" )
        assertEquals( bufferContent(), "0123\n4567\n89AB\nCDEF" )
        assertEquals(mode(), MODE_NORMAL)
        assertPos( 4, 4)

        sendkeys( "<RIGHT>" )
        sendkeys( "<DOWN>" )
        assertPos( 4, 4 )

        sendkeys( "<LEFT>" )
        assertPos( 4, 3 )

        sendkeys( "<UP>" )
        assertPos( 3, 3 )

        sendkeys( "<UP>" )
        sendkeys( "<UP>" )
        sendkeys( "<UP>" )
        assertPos( 1, 3 )
        sendkeys( "<UP>" )
        assertPos( 1, 3 )

        sendkeys( "<LEFT>" )
        sendkeys( "<LEFT>" )
        assertPos( 1, 1 )
        sendkeys( "<LEFT>" )
        assertPos( 1, 1 )
        sendkeys( "<DOWN>" )
        assertPos( 2, 1 )

        sendkeys( "2<RIGHT>" )
        sendkeys( "<RIGHT>" )
        sendkeys( "2<DOWN>" )
        assertPos( 4, 4 )

        sendkeys( "10<LEFT>" )
        assertPos( 4, 1 )
        sendkeys( "10<UP>" )
        assertPos( 1, 1 )
        sendkeys( "10<RIGHT>" )
        assertPos( 1, 4 )
        sendkeys( "10<DOWN>" )
        assertPos( 4, 4 )

        -- now with hjkl
        sendkeys( "l" )
        sendkeys( "j" )
        assertPos( 4, 4 )

        sendkeys( "h" )
        assertPos( 4, 3 )

        sendkeys( "k" )
        assertPos( 3, 3 )

        sendkeys( "3k" )
        assertPos( 1, 3 )
        sendkeys( "k" )
        assertPos( 1, 3 )

        sendkeys( "2h" )
        assertPos( 1, 1 )
        sendkeys( "h" )
        assertPos( 1, 1 )
        sendkeys( "j" )
        assertPos( 2, 1 )

        sendkeys( "3l" )
        sendkeys( "2j" )
        assertPos( 4, 4 )
    end

	function TestMovements:testBeginEndCharMovement()
		set("nocindent")
	    sendkeys( "i<TAB>0123<ENTER>4567<ENTER>  89AB <ESC>" )
	    assertPos( 3, 7 )
	
	    -- test beginning and end of line movements
	    sendkeys("^")
	    assertPos( 3, 3 )
	    sendkeys("0")
	    assertPos( 3, 1 )
	    sendkeys("$")
	    assertPos( 3, 7 )
	
	    sendkeys("<UP>")
	    sendkeys("0")
	    assertPos( 2, 1 )
	    sendkeys("^")
	    assertPos( 2, 1 )
	    sendkeys("$")
	    assertPos( 2, 4 )
	
	    sendkeys("<UP>")
	    sendkeys("0")
	    assertPos( 1, 1 )
	    sendkeys("^")
	    assertPos( 1, 2 )
	    sendkeys("$")
	    assertPos( 1, 5 )
	    set("cindent&")
	end

	function TestMovements:testLineMovement()
		--we test 'gg' like commands, make sure :set startofline=true first
		set("startofline")
	    sendkeys( "i<TAB><TAB>0123<ENTER>4567<ENTER>89AB<ENTER> CDEF<ESC>" )
	    assertEquals( bufferContent(), "\t\t0123\n4567\n89AB\n CDEF" )
	    assertEquals(mode(), MODE_NORMAL)
	    assertPos( 4, 5 )
	
	    sendkeys( "gg" )
	    assertPos( 1, 3 ) --depends on :set startofline value
	    sendkeys( "<RIGHT>" )
	    assertPos( 1, 4 )
	    sendkeys( "gg" )
	    assertPos( 1, 3 )
	
	    sendkeys( "G" )
	    assertPos( 4, 2 )
	    sendkeys( "<RIGHT>" )
	    assertPos( 4, 3 )
	    sendkeys( "G" )
	    assertPos( 4, 2 )
	
	    sendkeys("0gg")
	    assertPos( 1, 3 )
	    sendkeys( "300G" )
	    assertPos( 4, 2 )
	    sendkeys("2gg")
	    assertPos( 2, 1 )
	    sendkeys( "300gg" )
	    assertPos( 4, 2 )
	    sendkeys("3G")
	    assertPos( 3, 1 )
	
	    sendkeys("$")
	    assertPos( 3, 4 )
	    sendkeys("0")
	    assertPos( 3, 1 )
	
	    --tests with startofline to false now
	    set("nostartofline")
	    sendkeys("gg")
	    assertPos( 1, 1 )
	    sendkeys("G")
	    assertPos( 4, 1 )

	    set("startofline&")
	end
	
	function TestMovements:testMotionMovement()
	    set("startofline")
	    --tests with spaces on one line
	    sendkeys( "iword1 word02 word03 word4 word05<ESC>" )
	    assertEquals( bufferContent(), "word1 word02 word03 word4 word05" )
	    sendkeys("gg")
	    assertPos( 1, 1 )
	    sendkeys("w")
	    assertPos( 1, 7 )
	    sendkeys("w")
	    assertPos( 1, 14 )
	    sendkeys("b")
	    assertPos( 1, 7 )
	    sendkeys("2w")
	    assertPos( 1, 21)
	    sendkeys("2b")
	    assertPos( 1, 7)
	    sendkeys("99b")
	    assertPos( 1, 1)
	    sendkeys("99w")
	    assertPos( 1, 32)
	    sendkeys( "dd" )
	
	    --tests with delimiters on one line
	    sendkeys( "itest/function(test)/method()/test()<ESC>" )
	    assertEquals( bufferContent(), "test/function(test)/method()/test()" )
	    sendkeys("gg")
	    assertPos( 1, 1 )
	    sendkeys("w")
	    assertPos( 1, 5 )
	    sendkeys("w")
	    assertPos( 1, 6 )
	    sendkeys("b")
	    assertPos( 1, 5 )
	    sendkeys("2w")
	    assertPos( 1, 14)
	    sendkeys("2b")
	    assertPos( 1, 5)
	    sendkeys("99b")
	    assertPos( 1, 1)
	    sendkeys("9w")
	    assertPos( 1, 34 )
	    sendkeys("w")
	    assertPos( 1, 35 )
	    sendkeys( "dd" )
	
	    --tests with spaces on multiple lines
	    sendkeys( "itest1 test02 test003 test04 test5<ENTER>test1 test002 test03 test004 test5<ENTER> test1 test2 <ESC>" )
	    assertEquals( bufferContent(), "test1 test02 test003 test04 test5\ntest1 test002 test03 test004 test5\n test1 test2 " )
	    sendkeys("gg")
	    assertPos( 1, 1 )
	    sendkeys("5w")
	    assertPos( 2, 1 )
	    sendkeys("gg")
	    assertPos( 1, 1 )
	    sendkeys("10w")
	    assertPos( 3, 2 )
	    sendkeys("gg")
	    assertPos( 1, 1 )
	    sendkeys("11w")
	    assertPos( 3, 8 )
	    sendkeys("w")
	    assertPos( 3, 13 )
	    --test backward
	    sendkeys("G$")
	    assertPos( 3, 13 )
	    sendkeys("5b")
	    assertPos( 2, 15 )
	    sendkeys("G$")
	    assertPos( 3, 13 )
	    sendkeys("10b")
	    assertPos( 1, 14 )
	    sendkeys("G$")
	    assertPos( 3, 13 )
	    sendkeys("11b")
	    assertPos( 1, 7 )
	    sendkeys("b")
	    assertPos( 1, 1 )
	
	    --tests with delimiters on multiple lines
	    set("nocindent")
		clearBuffer();
	    sendkeys( "itest/function(test)/class::method()/<ENTER><TAB>void yzis::method(test()){<ENTER><TAB><TAB>printf(truc)<ESC>" )
	    assertEquals( bufferContent(), "test/function(test)/class::method()/\n\tvoid yzis::method(test()){\n\t\tprintf(truc)" )
	    sendkeys("gg")
	    assertPos( 1, 1 )
	    sendkeys("10w")
	    assertPos( 2, 2 )
	    sendkeys("8w")
	    assertPos( 3, 9 )
	    sendkeys("5b")
	    assertPos( 2, 13 )
	    --cleanup
		clearBuffer()
	    set("startofline&")
	    set("cindent&")
	end

	function TestMovements:testStickyColumn()
		clearBuffer()
		insertline(1, "line1 line1 line1 line1 line1")
		insertline(2, "		line2 line2 line2 line2")
		goto(1,1)
		assertPos(1,1)
		sendkeys("2lj")
		assertScrPos(2,1)
		assertPos(2,1)
		sendkeys("k")
		assertScrPos(1,3)
		assertPos(1,3)
		sendkeys("8lj")
		assertScrPos(2,9)
		assertPos(2,2)
		sendkeys("k")
		assertScrPos(1,11)
		assertPos(1,11)
		sendkeys("8l")
		assertPos(1,19)
		sendkeys("j")
		assertScrPos(2,19)
		assertPos(2,5)
		sendkeys("k")
		assertScrPos(1,19)
		sendkeys("j$")
		assertPos(2,25)
		assertScrPos(2,39)
		sendkeys("k")
		assertPos(1,29)
		assertScrPos(1,29)
	end
	

if not _REQUIREDNAME then
    -- LuaUnit:run('TestLuaBinding:test_setline') -- will execute only one test
    -- LuaUnit:run('TestLuaBinding') -- will execute only one class of test
    LuaUnit:run() -- will execute all tests
end

