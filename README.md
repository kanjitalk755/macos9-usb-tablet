Add this to your QEMU command line:

	-device usb-tablet -prom-env 'use-nvramrc?=true' -prom-env nvramrc=\
	': b64 do 0 4 0 do 6 lshift i j + c@ 2e - dup b > 7 and - dup 25 > 6 and - or'\
	' loop 3 0 do dup c, 8 rshift loop drop 4 +loop ; variable lzd variable lzi :'\
	' lzp c@ dup c, lzd @ lzi @ + c! lzi @ 1+ fff and lzi ! ; : lz lzd ! fee lzi '\
	'! 1 -rot do dup 1 = if drop i c@ 100 + 1 else dup 2 / swap 1 and if i lzp 1 '\
	'else i 1+ c@ i c@ over f0 and 4 lshift + swap f and -3 do dup lzd @ + lzp 1+'\
	' fff and loop drop 2 then then +loop drop ; 20 parse PofzQ03tNaNZRr1z.4Bk.E.'\
	'.YfHbwDfo.kU/oU6.zk2/.E.K5ik2.6.6.U4fA.YJ4EEuQD1v.E8b0FI1Y.I/YD1v.EHvT.A/dU6'\
	'6cTyzYDHz0../6TyICv1zzzz.TzxwgrUPbUj.MDwsC4FWpOu.MzwwG5BpZE..ETy.U/E.fU6.Thx'\
	'wULUP.4mSzk.AU2/.C.k.zTxU.7PCM1VUClU..81L.7MM.Ne/zk6tTkk.dUCdzmAtCTzzzzw6zkW'\
	'Ab.2..E.7zk/0UTXz2..zzk.tDG3tPoc6zsa2T.E..2.2zs7.YzXz0U11zmCFU/6.e.KUjuCEY.E'\
	'.e.JUU.4m..5z6HVMT3...kXjQviaU2s.68g..Di/.NiE.0gAx5tA.kY.U2ua2GY29jHh6.ET0k2'\
	'/zlUV/ME5M.k/zos4gG9t/6HwDqM/j0.4ua.W3UE/Nu66TV./.F2QN.3bPD1uwDd0138A2EI/3A2'\
	'0./2BW/HR...G.E22zk3YG.38.2M.zqt7QaJoMq3azolZ.47dRqJCzrFEFk/mIbFZ5rBZQbJj.KJ'\
	'Xzys0NUw..UgG7nts.Eog6/d24i039JcU1ogWZW2OmEA./KNU..0uPV2azo20RiXxUu.2vDzW4zk'\
	't.CMUEXgM9/k/..bzUXg.Clk..Q7zUY2AhWU..JwU.C.s.9s06A61hYGJkU6UhaUU6R2UGDZciU/'\
	'5B2U.zDxXb5nzT0sM9VaT.Dfyc1XoTk2.yqzXjt/s.E5..NeR6COqunh2SnQUq05k6DJu6jbN.Dy'\
	'EAE6M5t5zamk.D.0z6CPu.de35..TjlmEU/k.2./z21.K.mnr.DY.WAU..DySzcU/W.A..Dxy5cY'\
	'0I.E.FDxYc8.iIG.AFDw5682iT06A8Tz2FrrKJBN7Wzz4GpLyT/s6KvzAU73sXWU6zmkk0607U7b'\
	'Djws6./u6acY.0k/gB.1r6/cj.6/xJTwoThZgqE/IejdykU..E4.U1.0O61ajZc..lUU.zk.k.G0'\
	'NcHVmvXU..1/WP6.s.4A.JDwoThZX.4AsJ8w/DUFXtE0KWE4O.1tz63I.kfo54DwU.64/P8.s31w'\
	'.24BU0v..ATd.CdavG19Qw6EorvkUT0sUyG4z.A6X.flxtcUlYk..zn.5.ByHTtAUfmE..Dyk.7M'\
	'o.deV..g/2/dc2VtUYW0P/5MZCQ1vU00vzDzWnDyEW/k..U0YdzqMMU.fGynz8Tvz6Ms7G/.KE/.'\
	'O7V2St2YIE/x4ei.G2Y/DHw9Q2Y1WHxfc6FBM6FAVUazc.Ek0.E1s6a.k..Vr6/cYr.2Yzp2UG..'\
	'X.Ms/zWU1Um.PRE22N47n.c5T7kM2.k7b8Dw0FJ25J3JBMjxV.GAO.c...Ty02EU0A020Ulw2C.6'\
	'//4QkPUIKaWVR2RoIV.3ba0.OAm.Pk6VE466P40IVgmcY3AtI2Ocb2HQ.pYo.rUcIrkAIU.F0L2M'\
	'1.E2UZM1h.I6.I/fezqxEFLFnPaJqzk/oEZBJQaJHtqZqQqJX5V5nOINUNDxiS4JCOJ/oNTpkEq2'\
	'RNatjlqRdNL7p5FPe2CdVjqJG.4FVNKVIF4.SOL9zQaJqQqJ2QaDzR5/dPaxdM3uyMKl1N5Bnzp/'\
	'XNrJgF4tdrrBdR43kI4VXNKlEI/vy3k...CFHvugV.4R.6/dMIDq6.1g.6/dj.E16 over + swa'\
	'p here >r 2000 allot here >r b64 here r> here r@ here! lz here r> tuck - dev'\
	' / " pef,AAPL,MacOS,PowerPC,prepare" property " Tab" encode-string " code,AA'\
	'PL,MacOS,name" property'

Do not add `via=pmu` to the QEMU argument `-M` as it will not work properly.
