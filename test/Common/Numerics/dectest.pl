#! /usr/bin/perl -w

# cls && perl test/Common/Numerics/dectest.pl ^
#   %d%\dqAdd.decTest %d%\dqSubtract.decTest ^
#   %d%\dqMultiply.decTest %d%\dqDivide.decTest ^
#   %d%\dqDivideInt.decTest ^
#   > test\Common\Numerics\Decimal128MoreTest.cpp

use integer;
use strict;
no warnings 'uninitialized';

# Expected settings.
my $PRECISION = 34;
my $ROUNDING = 'half_even';

my $Excludes = {
  map { $_ => 1 } qw(
    dqadd014
    dqadd015
    dqadd016
    dqadd017
    dqadd225
    dqadd226
    dqadd227
    dqadd240
    dqadd241
    dqadd6565
    dqadd6566
    dqadd6567
    dqadd7540
    dqadd7541
    dqadd71300
    dqadd71350
    dqadd71351
    dqadd71352
    dqadd71353
    dqadd71354
    dqadd71355
    dqadd71356
    dqadd71357
    dqadd71358
    dqadd71359
    dqadd71360
    dqadd71361
    dqadd71362
    dqadd71363
    dqadd71364
    dqadd75040
    dqadd75041
    dqadd75049
    dqadd75057
    dqdiv1810
    dqdiv1813
    dqdiv1820
    dqdiv1826
    dqdvi531
    dqmul810
    dqmul813
    dqmul820
    dqmul826
    dqmul907
    dqmul908
    dqsub472
    dqsub528
    dqsub529
    dqsub530
    dqsub531
    dqsub542
    dqsub628
    dqsub629
    dqsub630
    dqsub631
    dqsub642
  )
};

my $MapOp = {
  'add' => 'Add',
  'divide' => 'Div',
  'divideint' => 'DivInt',
  'multiply' => 'Mul',
  'subtract' => 'Sub',
};

my $CaseMap = {};
my $LastSettings = {};
my $Settings = {};
my $SkippedOps = {};

sub emitTestCases {
  map {
    my $c_name = $_;
    my $cases = $CaseMap->{$c_name};
    printf STDERR "Emit %d test cases for %s\n", scalar(@$cases), $c_name;
    printf "\nTEST_F(Decimal128MoreTest, %s) {\n", $c_name;
    map { map { printf "  %s\n", $_ } @$_ } @$cases;
    printf "}\n";
  } sort { $a cmp $b } keys(%$CaseMap);
}

sub emitTestScript {
  my $date_time = scalar(gmtime);
  while (<DATA>) {
    chop;
    s{__DATATIME__}{$date_time};
    if (m{---}) {
      emitTestCases();
    } else {
      print $_, "\n";
    }
  }
}

sub equalSettings {
  my ($map1, $map2) = @_;
  map { return undef unless $map1->{$_} eq $map2->{$_} } keys(%$map1);
  map { return undef unless $map1->{$_} eq $map2->{$_} } keys(%$map2);
  return 1;
}

sub makeTestCase {
  my ($data) = @_;

  my $c_name = $MapOp->{$data->{operation}};
  return undef unless $c_name;

  my $params = $data->{params};
  return undef unless defined($params->[0]) && defined($params->[1]);

  my $lines = [];

  unless (equalSettings($LastSettings, $Settings)) {
    map {
      my $last = $LastSettings->{$_};
      my $curr = $Settings->{$_};
      unless ($last eq $curr) {
        push @$lines, sprintf("// %s: %s", $_, $curr);
        $LastSettings->{$_} = $curr;
      }
    } keys(%$Settings);
  }

  my $test_id = $data->{id};

  return unless $Settings->{precision} eq $PRECISION;
  return unless $Settings->{rounding} eq $ROUNDING;

  push @$lines, sprintf(
    "%sEXPECT_STREQ(L\"%s\", %s(\"%s\", \"%s\").value()) << \"%s\";",
    $Excludes->{$test_id} ? "// " : q{},
    formatParam($data->{result}),
    $c_name,
    formatParam($params->[0]),
    formatParam($params->[1]),
    $test_id);

  push @{$CaseMap->{$c_name}}, $lines;
  return 1;
}

sub formatParam {
  my $param = shift;
  return 'NaN' if $param eq '#';
  $param =~ s{\x27}{}g;
  $param =~ s{NaN\d+}{NaN}g;
  $param =~ s{-[.]}{-0.};
  $param =~ s{[.][Ee]}{E}; # Remove dot before exponent marker
  $param =~ s{^[.]}{0.}; # Insert "0" before starting dot
  $param =~ s{[.]$}{}; # Remove trailing dot
  return $param;
}

sub main {
  my $num_cases = 0;
  my $num_lines = 0;
  my $case_name = undef;
  while (<>) {
    ++$num_lines;
    s{--.*}{}g;
    next if !length;

    if (my ($keyword, $param) = m{^(.*?):\s*(.*?)\s*$}) {
      if ($keyword eq 'version') {
        map { delete $Settings->{$_} } keys(%$Settings);
        map { delete $LastSettings->{$_} } keys(%$LastSettings);
      }

      $Settings->{$keyword} = $param;
      next;
    }

    my @words = split(m{\s+});

    my $data = {
      id => shift(@words),
      operation => shift(@words),
      params => [],
      result => undef,
      conditions => [],
    };

    my $state = 'params';
    for my $word (@words) {
      if ($state eq 'params') {
        if ($word eq '->') {
          $state = 'result';
        } else {
          push @{$data->{params}}, $word
        }
      } elsif ($state eq 'result') {
        $data->{result} = $word;
        $state = 'conditions';

      } elsif ($state eq 'conditions') {
        push @{$data->{conditions}}, $word;
      } else {
        die "Unknown state($state)";
      }
    }

    my $operation = $data->{operation};
    next unless $operation;

    if (makeTestCase($data)) {
      ++$num_cases;
    } else {
      my $key = sprintf("%s %s", $Settings->{rounding}, $operation);
      unless ($SkippedOps->{$key}) {
        $SkippedOps->{$key} = 1;
        printf STDERR "%s: Ignore rounding(%s) operation(%s)\n",
            $num_lines,
            $Settings->{rounding},
            $operation;
      }
    }
  }
  emitTestScript();
  printf STDERR "Generate %d test cases (%d skipped) by %s\n",
      $num_cases, scalar(keys(%$Excludes)), $0;
  return 0;
}

exit main();
__DATA__
#include "precomp.h"
// Evita Common Decimal128 Extended Test
// This test script was generated by a script.
// You should not edit this file.
// We take test cases from http://speleotrove.com/decimal/
// Generated at __DATATIME__
#include "./AbstractDecimal128Test.h"

namespace CommonTest {
class Decimal128MoreTest : public AbstractDecimal128Test {
};
---

} // CommonTest
