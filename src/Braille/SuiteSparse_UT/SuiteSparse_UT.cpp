// Source file that uses suitesparse package to do something interesting

#include <CppUnitTest.h>
#include <suitesparse\cs.h>

namespace sourcesdep_ss
{
    using namespace Microsoft::VisualStudio::CppUnitTestFramework;

    TEST_CLASS(SuiteSparse_Tests)
    {
    public:
        void MakeCycleMessage(const cs_did* dmperm_result)
        {
            int ns, k;
            for (ns = 0, k = 0; k < dmperm_result->nb; k++)
            {
                if (!((dmperm_result->r[k + 1] == dmperm_result->r[k] + 1) &&
                    (dmperm_result->s[k + 1] == dmperm_result->s[k] + 1)))
                {
                    // this represent a cycle, print a numeric representation of the cycle
                    std::ostringstream message;
                    message << "Cycles found: \r\n";
                    for (int cycleIndex = dmperm_result->r[k]; cycleIndex < dmperm_result->r[k + 1]; cycleIndex++)
                    {
                        message << dmperm_result->p[cycleIndex];
                        if (cycleIndex < dmperm_result->r[k + 1] - 1)
                        {
                            message << " -> ";
                        }
                    }
                    message << "\r\n";
                    printf("%s", message.str().c_str());
                }
            }
        }

        TEST_METHOD(SuiteSparse_Basic_OnlySelfDependencies)
        {
            int graphSize = 5;
            // param 3 is initial allocation size but it will grow as needed by doubling allocation
            // param 5 controls is the data is triplet or not
            auto spSpace = cs_spalloc(graphSize, graphSize, graphSize, 0, 1);
            Assert::IsNotNull(spSpace);

            //make everything depend on itself
            for (int index = 0; index < graphSize; index++)
            {
                Assert::AreEqual(1, cs_entry(spSpace, index, index, 0));
            }

            cs_di* csCompressed = cs_compress(spSpace);
            Assert::IsNotNull(csCompressed);
            cs_spfree(spSpace);
            spSpace = nullptr;

            cs_did* dmperm_result = cs_dmperm(csCompressed, 1);
            Assert::IsNotNull(dmperm_result);
            int k, ns;
            //int sprank = dmperm_result->rr[3];
            for (ns = 0, k = 0; k < dmperm_result->nb; k++)
            {
                ns += ((dmperm_result->r[k + 1] == dmperm_result->r[k] + 1) &&
                    (dmperm_result->s[k + 1] == dmperm_result->s[k] + 1));
            }

            // Validate we have graphSize single entry blocks
            Assert::AreEqual(graphSize, ns);
            // Validate we have graphsize blocks since each entry only depends on itself
            Assert::AreEqual(graphSize, dmperm_result->nb);
        }

        TEST_METHOD(SuiteSparse_Basic_NoCycles)
        {
            int graphSize = 5;
            // param 3 is initial allocation size but it will grow as needed by doubling allocation
            // param 5 controls is the data is triplet or not
            auto spSpace = cs_spalloc(graphSize, graphSize, graphSize, 0, 1);
            Assert::IsNotNull(spSpace);

            //make everything depend on itself
            for (int index = 0; index < graphSize; index++)
            {
                Assert::AreEqual(1, cs_entry(spSpace, index, index, 0));
                // Add dependency on next element unless the last element. Each node but the last depends on the next node
                if (index + 1 < graphSize)
                {
                    Assert::AreEqual(1, cs_entry(spSpace, index, index + 1, 0));
                }
            }

            cs_di* csCompressed = cs_compress(spSpace);
            Assert::IsNotNull(csCompressed);
            cs_spfree(spSpace);
            spSpace = nullptr;

            cs_did* dmperm_result = cs_dmperm(csCompressed, 1);
            Assert::IsNotNull(dmperm_result);
            int k, ns;
            for (ns = 0, k = 0; k < dmperm_result->nb; k++)
            {
                ns += ((dmperm_result->r[k + 1] == dmperm_result->r[k] + 1) &&
                    (dmperm_result->s[k + 1] == dmperm_result->s[k] + 1));
            }

            // Validate ns is gaph size. It must be since it corelates to nb. Since we have no cycles it must still be equal to nb
            Assert::AreEqual(graphSize, ns);
            // Validate we have graphsize blocks as there are no cycles
            Assert::AreEqual(graphSize, dmperm_result->nb);
        }

        TEST_METHOD(SuiteSparse_Basic_OneCycle_TwoNodes)
        {
            //Create a matrix with 
            int graphSize = 5;
            // param 3 is initial allocation size but it will grow as needed by doubling allocation
            // param 5 controls is the data is triplet or not
            auto spSpace = cs_spalloc(graphSize, graphSize, graphSize, 0, 1);
            Assert::IsNotNull(spSpace);

            //make everything depend on itself
            for (int index = 0; index < graphSize; index++)
            {
                Assert::AreEqual(1, cs_entry(spSpace, index, index, 0));
            }

            //form two node cycle between 0 -> 1 -> 0
            Assert::AreEqual(1, cs_entry(spSpace, 0, 1, 0));
            Assert::AreEqual(1, cs_entry(spSpace, 1, 0, 0));

            cs_di* csCompressed = cs_compress(spSpace);
            Assert::IsNotNull(csCompressed);
            cs_spfree(spSpace);
            spSpace = nullptr;

            cs_did* dmperm_result = cs_dmperm(csCompressed, 1);
            Assert::IsNotNull(dmperm_result);
            int k, ns;
            for (ns = 0, k = 0; k < dmperm_result->nb; k++)
            {
                ns += ((dmperm_result->r[k + 1] == dmperm_result->r[k] + 1) &&
                    (dmperm_result->s[k + 1] == dmperm_result->s[k] + 1));
            }

            // We should have 3 1 line entries
            Assert::AreEqual(3, ns);
            // Validate we have 4 blocks as one block represents a cycle
            Assert::AreEqual(4, dmperm_result->nb);
        }

        TEST_METHOD(SuiteSparse_Basic_OneCycle_ThreeNodes)
        {
            //Create a matrix with 
            int graphSize = 5;
            // param 3 is initial allocation size but it will grow as needed by doubling allocation
            // param 5 controls is the data is triplet or not
            auto spSpace = cs_spalloc(graphSize, graphSize, graphSize, 0, 1);
            Assert::IsNotNull(spSpace);

            //make everything depend on itself
            for (int index = 0; index < graphSize; index++)
            {
                Assert::AreEqual(1, cs_entry(spSpace, index, index, 0));
            }

            //form thre node cycle between 0 -> 1 -> 4 -> 0
            Assert::AreEqual(1, cs_entry(spSpace, 0, 1, 0));
            Assert::AreEqual(1, cs_entry(spSpace, 1, 4, 0));
            Assert::AreEqual(1, cs_entry(spSpace, 4, 0, 0));

            cs_di* csCompressed = cs_compress(spSpace);
            Assert::IsNotNull(csCompressed);
            cs_spfree(spSpace);
            spSpace = nullptr;

            cs_did* dmperm_result = cs_dmperm(csCompressed, 1);
            Assert::IsNotNull(dmperm_result);
            int k, ns;
            for (ns = 0, k = 0; k < dmperm_result->nb; k++)
            {
                ns += ((dmperm_result->r[k + 1] == dmperm_result->r[k] + 1) &&
                    (dmperm_result->s[k + 1] == dmperm_result->s[k] + 1));
            }

            // We should have 2 1 line entries
            Assert::AreEqual(2, ns);
            // Validate we have 3 blocks as one block represents a cycle of 3 nodes
            Assert::AreEqual(3, dmperm_result->nb);

            MakeCycleMessage(dmperm_result);
        }

        TEST_METHOD(SuiteSparse_Basic_OneCycle_TwoNodes_ExtraDependencies)
        {
            //Create a matrix with 
            int graphSize = 5;
            // param 3 is initial allocation size but it will grow as needed by doubling allocation
            // param 5 controls is the data is triplet or not
            auto spSpace = cs_spalloc(graphSize, graphSize, graphSize, 0, 1);
            Assert::IsNotNull(spSpace);

            //make everything depend on itself
            for (int index = 0; index < graphSize; index++)
            {
                Assert::AreEqual(1, cs_entry(spSpace, index, index, 0));
            }

            //form two node cycle between 0 -> 1 -> 0
            Assert::AreEqual(1, cs_entry(spSpace, 0, 1, 0));
            Assert::AreEqual(1, cs_entry(spSpace, 1, 0, 0));

            //form extra dependencies on 0 to understand how we generate the cycle message
            Assert::AreEqual(1, cs_entry(spSpace, 0, 2, 0));
            Assert::AreEqual(1, cs_entry(spSpace, 0, 4, 0));

            cs_di* csCompressed = cs_compress(spSpace);
            Assert::IsNotNull(csCompressed);
            cs_spfree(spSpace);
            spSpace = nullptr;

            cs_did* dmperm_result = cs_dmperm(csCompressed, 1);
            Assert::IsNotNull(dmperm_result);
            int k, ns;
            for (ns = 0, k = 0; k < dmperm_result->nb; k++)
            {
                ns += ((dmperm_result->r[k + 1] == dmperm_result->r[k] + 1) &&
                    (dmperm_result->s[k + 1] == dmperm_result->s[k] + 1));
            }

            // We should have 3 1 line entries
            Assert::AreEqual(3, ns);
            // Validate we have 4 blocks as one block represents a cycle
            Assert::AreEqual(4, dmperm_result->nb);

            MakeCycleMessage(dmperm_result);
        }

        TEST_METHOD(SuiteSparse_Basic_TwoCycles_TwoNodes)
        {
            //Create a matrix with 
            int graphSize = 5;
            // param 3 is initial allocation size but it will grow as needed by doubling allocation
            // param 5 controls is the data is triplet or not
            auto spSpace = cs_spalloc(graphSize, graphSize, graphSize, 0, 1);
            Assert::IsNotNull(spSpace);

            //make everything depend on itself
            for (int index = 0; index < graphSize; index++)
            {
                Assert::AreEqual(1, cs_entry(spSpace, index, index, 0));
            }

            //form two node cycle between 0 -> 1 -> 0
            Assert::AreEqual(1, cs_entry(spSpace, 0, 1, 0));
            Assert::AreEqual(1, cs_entry(spSpace, 1, 0, 0));

            //form two node cycle between 3 -> 4 -> 3
            Assert::AreEqual(1, cs_entry(spSpace, 3, 4, 0));
            Assert::AreEqual(1, cs_entry(spSpace, 4, 3, 0));

            cs_di* csCompressed = cs_compress(spSpace);
            Assert::IsNotNull(csCompressed);
            cs_spfree(spSpace);
            spSpace = nullptr;

            cs_did* dmperm_result = cs_dmperm(csCompressed, 1);
            Assert::IsNotNull(dmperm_result);
            int k, ns;
            for (ns = 0, k = 0; k < dmperm_result->nb; k++)
            {
                ns += ((dmperm_result->r[k + 1] == dmperm_result->r[k] + 1) &&
                    (dmperm_result->s[k + 1] == dmperm_result->s[k] + 1));
            }

            // We should have 1 1 line entries (2 cycles of 2 nodes = 4 lines)
            Assert::AreEqual(1, ns);
            // Validate we have 4 blocks as one block represents a cycle
            Assert::AreEqual(3, dmperm_result->nb);

            MakeCycleMessage(dmperm_result);
        }

        TEST_METHOD(SuiteSparse_FullGraphCycle)
        {
            //Create a matrix with 
            int graphSize = 5;
            // param 3 is initial allocation size but it will grow as needed by doubling allocation
            // param 5 controls is the data is triplet or not
            auto spSpace = cs_spalloc(graphSize, graphSize, graphSize, 0, 1);
            Assert::IsNotNull(spSpace);

            //make everything depend on itself and n+1 % graphSize (so 4 -> 0
            for (int index = 0; index < graphSize; index++)
            {
                Assert::AreEqual(1, cs_entry(spSpace, index, index, 0));
                Assert::AreEqual(1, cs_entry(spSpace, index, (index + 1) % graphSize, 0));
            }

            cs_di* csCompressed = cs_compress(spSpace);
            Assert::IsNotNull(csCompressed);
            cs_spfree(spSpace);
            spSpace = nullptr;

            cs_did* dmperm_result = cs_dmperm(csCompressed, 1);
            Assert::IsNotNull(dmperm_result);
            int k, ns;
            for (ns = 0, k = 0; k < dmperm_result->nb; k++)
            {
                ns += ((dmperm_result->r[k + 1] == dmperm_result->r[k] + 1) &&
                    (dmperm_result->s[k + 1] == dmperm_result->s[k] + 1));
            }

            // We should have 0 1 line entries
            Assert::AreEqual(0, ns);
            // Validate we have 1 block as the entire graph is a cycle
            Assert::AreEqual(1, dmperm_result->nb);

            MakeCycleMessage(dmperm_result);
        }

        TEST_METHOD(SuiteSparse_FullGraphCycle_WithSubCycles)
        {
            //Create a matrix with 
            int graphSize = 5;
            // param 3 is initial allocation size but it will grow as needed by doubling allocation
            // param 5 controls is the data is triplet or not
            auto spSpace = cs_spalloc(graphSize, graphSize, graphSize, 0, 1);
            Assert::IsNotNull(spSpace);

            //make everything depend on itself and n+1 % graphSize (so 4 -> 0
            for (int index = 0; index < graphSize; index++)
            {
                Assert::AreEqual(1, cs_entry(spSpace, index, index, 0));
                Assert::AreEqual(1, cs_entry(spSpace, index, (index + 1) % graphSize, 0));
            }

            //form two node cycle between 0 -> 1 -> 0
            Assert::AreEqual(1, cs_entry(spSpace, 1, 0, 0));

            //form two node cycle between 3 -> 4 -> 3
            Assert::AreEqual(1, cs_entry(spSpace, 4, 3, 0));

            cs_di* csCompressed = cs_compress(spSpace);
            Assert::IsNotNull(csCompressed);
            cs_spfree(spSpace);
            spSpace = nullptr;

            cs_did* dmperm_result = cs_dmperm(csCompressed, 1);
            Assert::IsNotNull(dmperm_result);
            int k, ns;
            for (ns = 0, k = 0; k < dmperm_result->nb; k++)
            {
                ns += ((dmperm_result->r[k + 1] == dmperm_result->r[k] + 1) &&
                    (dmperm_result->s[k + 1] == dmperm_result->s[k] + 1));
            }

            // We should have 0 1 line entries
            Assert::AreEqual(0, ns);
            // Validate we have 3 block as the entire graph is a cycle plus two other cycles
            Assert::AreEqual(1, dmperm_result->nb);

            MakeCycleMessage(dmperm_result);
        }
    };
}