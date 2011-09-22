 select * FROM icd_9_descriptions JOIN cm_gems ON icd_9_descriptions.icd_9_code = cm_gems.icd_9_code JOIN icd_10_cm_descriptions ON cm_gems.icd_10_code = icd_10_cm_descriptions.icd_10_code where cm_gems.icd_10_code = 'A0229';
 select * FROM icd_9_descriptions i_9 JOIN cm_gems g ON i_9.icd_9_code = g.icd_9_code JOIN icd_10_cm_descriptions i_10 ON g.icd_10_code = i_10.icd_10_code where g.icd_9_code = '00324';
 select * FROM icd_9_descriptions i_9 JOIN cm_gems g ON i_9.icd_9_code = g.icd_9_code JOIN icd_10_cm_descriptions i_10 ON g.icd_10_code = i_10.icd_10_code where g.icd_9_code = '0041';
 select * FROM icd_9_descriptions i_9 JOIN cm_gems g ON i_9.icd_9_code = g.icd_9_code JOIN icd_10_cm_descriptions i_10 ON g.icd_10_code = i_10.icd_10_code where g.icd_9_code = 'V222';
 select * FROM icd_9_descriptions i_9 JOIN cm_gems g ON i_9.icd_9_code = g.icd_9_code JOIN icd_10_cm_descriptions i_10 ON g.icd_10_code = i_10.icd_10_code where g.icd_9_code = '6259';
