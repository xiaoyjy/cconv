-- 0.6.2
> Add GB-Hans GB-Hant encoding which were used to represent GB charset in Simplified and Traditional.
> Use gb18030 handing all GB code.

> Support convert gbk to gb-hant or gb-hans.

-- 0.6.1
> Update cconv\_table.cc to latest.

> Add some Tongwen Tang word mapping.
> Fix the bug that something cconv output are not end with '\0'

-- 0.6.0
> Update cconv\_table.cc to latest.

> Fix the bug that Jane transfer into fan does not work.

-- 0.6.0\_beta
> Rewrite the cross-handling part, and change the judge algorithm.

> Increase four kinds of prefix & suffix based conditions conversion
> mode:
    1. convert the prefix match before only
    1. convert the suffix match before only
    1. prefix match is not to convert.
    1. suffix match is not to convert.

-- 0.5.2
> Add AC\_CONFIG\_MACRO\_DIR([m4](m4.md)) to fix complie error on CentOS 4.X

-- 0.5.1

-- 0.5.-